#include "..\Tools\Definitions.h"

StructuredBuffer<float> Parameters : register(t0);

//#define MAJORANT_OFFSET 0
//#define ALBEDO_OFFSET 1
//#define GFACTOR_OFFSET 2
#define DENSITY_OFFSET 0

#define GRID_SIZE RECONSTRUCTION_MAX_SIZE

cbuffer Camera : register(b0) {
	float4x4 FromProjectionToWorld;
};

// CB used to count passes of the PT and to know if the output image is a complexity image or the final color.
cbuffer AccumulativeInfo : register(b1) {
	uint NumberOfPasses;
	uint ShowComplexity;
	float PathtracingRatio;
	uint AnimatedFrame;
}

cbuffer Lighting : register(b2) {
	float3 LightPosition;
	float3 LightIntensity;
	float3 LightDirection;
}

// Output textures
RWTexture2D<float4> Output						: register(u0, space0); // Final Output image from the ray-trace
RWTexture2D<float3> Accumulation				: register(u1, space0); // Auxiliar Accumulation Buffer
RWTexture2D<float3> SqrAccumulation				: register(u2, space0); // Square accumulator (for variance estimation)
RWTexture2D<uint> Complexity					: register(u3, space0); // Complexity buffer for debuging

#include "..\Tools\CommonComplexity.h"

void AccumulateOutput(uint2 coord, float3 value, int complexity) {
	Accumulation[coord] += value;
	SqrAccumulation[coord] += value * value;
	Complexity[coord] += complexity;

	if (ShowComplexity)
		Output[coord] = float4(GetColor((int)round(Complexity[coord] / (NumberOfPasses + 1))), 1);
	else
		Output[coord] = float4(Accumulation[coord] / (NumberOfPasses + 1), 1);
}

// Random using is HybridTaus
#include "..\Tools\Randoms.h"
#include "..\Tools\CommonEnvironment.h"

// SCATTERING PHASE FUNCTION

#include "..\Tools\HGPhaseFunction.h"
//#include "..\Tools\MiePhase.h"

//float invertcdf(float GFactor, float xi)
//{
//	/*float one_minus_g2 = 1 - GFactor * GFactor;
//	float one_over_2g = 1.0f / (2 * GFactor);
//	float one_plus_g2 = 1 + GFactor * GFactor;*/
//
//	float t = (one_minus_g2) / (1.0f - GFactor + 2.0f * GFactor * xi);
//	return one_over_2g * (one_plus_g2 - t * t);
//}

float3x3 Rotate(float angle, float3 dir)
{
	float x = dir.x;
	float y = dir.y;
	float z = dir.z;
	float c = cos(angle);
	float s = sin(angle);

	return float3x3(
		x * x * (1 - c) + c, y * x * (1 - c) + z * s, z * x * (1 - c) - y * s, 
		x * y * (1 - c) - z * s, y * y * (1 - c) + c, z * y * (1 - c) + x * s, 
		x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, z * z * (1 - c) + c 
		);
}

void ImportanceSamplePhase(float GFactor, inout float3 D, inout float3 B, float xi1, float xi2)
{
	float phi = xi1 * 2 * pi;
	float cosTheta = clamp(-1, 1, abs(GFactor) < 0.001 ? 2 * xi2 - 1 : invertcdf(GFactor, xi2));
	float3x3 rotateB = Rotate(phi, D);
	B = normalize(mul(B, rotateB));
	float3x3 rotateD = Rotate(acos(cosTheta), B);
	D = normalize(mul(D, rotateD));
}

void ImportanceSamplePhaseBack(float GFactor, inout float3 D, inout float3 B, float xi1, float xi2)
{
	float phi = xi1 * 2 * pi;
	float cosTheta = clamp(-1, 1, abs(GFactor) < 0.001 ? 2 * xi2 - 1 : invertcdf(GFactor, xi2));
	float3x3 rotateD = Rotate(-acos(cosTheta), B);
	D = normalize(mul(D, rotateD));
	float3x3 rotateB = Rotate(-phi, D);
	B = normalize(mul(B, rotateB));
}

void GetGridBox(out float3 minim, out float3 maxim) {
	maxim = float3(1, 1, 1)*0.5;
	minim = -maxim;
}

bool BoxIntersect(float3 bMin, float3 bMax, float3 P, float3 D, inout float tMin, inout float tMax)
{
	float2x3 C = float2x3(bMin - P, bMax - P);
	float2x3 D2 = float2x3(D, D);
	float2x3 T = abs(D2) <= 0.000001 ? float2x3(float3(-1000, -1000, -1000), float3(1000, 1000, 1000)) : C / D2;
	tMin = max(max(min(T._m00, T._m10), min(T._m01, T._m11)), min(T._m02, T._m12));
	tMin = max(0.0, tMin);
	tMax = min(min(max(T._m00, T._m10), max(T._m01, T._m11)), max(T._m02, T._m12));
	return !(tMax <= tMin || tMax <= 0);
}

static int complexity;
static float3 Grid_Min, Grid_Max;
static int seed_increment;

float SampleGrid(float3 P) {
	complexity++;
	float3 coordinate = (P - Grid_Min) / (Grid_Max - Grid_Min);

	int3 coord = coordinate * GRID_SIZE;

	int index = coord.z * (GRID_SIZE * GRID_SIZE) + coord.y * GRID_SIZE + coord.x;

	return saturate(Parameters[DENSITY_OFFSET + index]);
}

float SampleGrid(int3 coord) {
	int index = coord.z * (GRID_SIZE * GRID_SIZE) + coord.y * GRID_SIZE + coord.x;
	return saturate(Parameters[DENSITY_OFFSET + index]);
}

float BoxExit(float3 bMin, float3 bMax, float3 x, float3 w)
{
	float2x3 C = float2x3(bMin - x, bMax - x);
	float2x3 D2 = float2x3(w, w);
	float2x3 T = abs(D2) <= 0.000001 ? float2x3(float3(-1000, -1000, -1000), float3(1000, 1000, 1000)) : C / D2;
	return min(min(max(T._m00, T._m10), max(T._m01, T._m11)), max(T._m02, T._m12));
}

void PTStep(inout float T, inout float3 x, inout float3 w, inout float3 B, float xi1, float xi2, float xi3, float xi4, out float t, out bool scatter) {
	float majorant = 100;// Parameters[MAJORANT_OFFSET];
	float g = 0.875 * 0.6;// Parameters[GFACTOR_OFFSET];
	float albedo = 1.0;// Parameters[ALBEDO_OFFSET];

	float3 xi = x;
	float3 wi = w;
	float3 Bi = B;
	float Ti = T;

	t = -log(1 - xi1) / majorant;
	float3 xo = xi + t * wi;

	float sigma_xo = SampleGrid(xo) * majorant;

	scatter = xi4 < sigma_xo / majorant;
	float3 wo = wi;
	float3 Bo = Bi;

	if (scatter) 
		ImportanceSamplePhase(g, wo, Bo, xi2, xi3);

	float To = (1 + scatter * (albedo - 1)) * Ti;

	T = To;
	x = xo;
	w = wo;
	B = Bo;
}

void createOrthoBasis(float3 N, out float3 T, out float3 B)
{
	float s = sign(N.z);
	float a = -1.0f / (s + N.z);
	float b = N.x * N.y * a;
	T = float3(1.0f + s * N.x * N.x * a, s * b, -s * N.x);
	B = float3(b, s + N.y * N.y * a, -N.y);
}

float BoxTransmittance(inout float3 x, inout float3 w) {
	float d = BoxExit(Grid_Min, Grid_Max, x, w);
	float T = 1;

	float3 Tang, B;
	createOrthoBasis(w, Tang, B);

	//int seed = random() / 2.3283064365387e-10;
	int seed = random() * 1000000000;// / 2.3283064365387e-10;

	while (true) {

		float4 xi = float4(random(), random(), random(), random());// randoms_by_seed(seed);
		seed += seed_increment;

		float t;
		bool scatter;
		PTStep(T, x, w, B, xi.x, xi.y, xi.z, xi.w, t, scatter);

		d -= t;

		if (d <= 0) // exit box
			return T;

		if (scatter)
			d = BoxExit(Grid_Min, Grid_Max, x, w);
	}
}

float3 Pathtrace(float3 x, float3 w)
{
	float tMin, tMax;
	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
		return 0;// (SampleSkybox(x, w) + SampleLight(w));
	float d = tMax - tMin;
	x += w * tMin;
	float3 win = w;
	float T = BoxTransmittance(x, w);
	float3 sky = 0;// SampleSkybox(x, w);
	return T * (sky + any(win-w)*SampleLight(w));
	//return lerp(float3(1, 0, 0), float3(0, 1, 0), pdf * 0.5);
}

float ComputeTransmittance(float3 x, float3 w) {
	float tMin, tMax;
	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
		return 1;
	float majorant = 100;// Parameters[MAJORANT_OFFSET];
	float d = tMax - tMin;
	x += w * tMin;

	int3 v = (x - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);
	float3 alpha_inc = 1 / (GRID_SIZE * max(0.000001, abs(w)));
	float3 side = w >= 0;
	float3 corner = (v + side) * (Grid_Max - Grid_Min) / GRID_SIZE + Grid_Min;
	float3 alpha = abs(corner - x) / max(0.000001, abs(w));

	float current_t = 0; 

	float tao = 0;

	int step = 0;
	while (current_t < d && step++ < 100) {
		float next_t = min(alpha.x, min(alpha.y, alpha.z));
		int3 v_inc = int3(
			alpha.x <= alpha.y && alpha.x <= alpha.z,
			alpha.x > alpha.y && alpha.y <= alpha.z,
			alpha.x > alpha.z && alpha.y > alpha.z);
		float dt = next_t - current_t;

		tao += SampleGrid(v) * majorant * dt;

		alpha += v_inc * alpha_inc;
		v += v_inc * (w >= 0 ? 1 : -1);
		current_t = next_t;
	}

	return exp(-tao);
}

float3 Transmittance(float3 x, float3 w) {
	return ComputeTransmittance(x,w) * (SampleSkybox(x,w));
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim;
	Output.GetDimensions(dim.x, dim.y);

	StartRandomSeedForRay(dim, 1, DTid.xy, 0, NumberOfPasses + (AnimatedFrame ^ 37) * 1000000);

	seed_increment = dim.x * dim.y * 47;

	float4 ndcP = float4(2 * (DTid.xy + float2(random(), random())) / dim - 1, 0, 1);
	ndcP.y *= -1;
	float4 ndcT = ndcP + float4(0, 0, 1, 0);

	float4 viewP = mul(ndcP, FromProjectionToWorld);
	viewP.xyz /= viewP.w;
	float4 viewT = mul(ndcT, FromProjectionToWorld);
	viewT.xyz /= viewT.w;

	float3 O = viewP.xyz;
	float3 D = normalize(viewT.xyz - viewP.xyz);

	GetGridBox(Grid_Min, Grid_Max);

	complexity = 0;

	float3 radiance = Transmittance(O, D);

	AccumulateOutput(DTid.xy, radiance, complexity);
}

