#include "..\Tools\Definitions.h"

StructuredBuffer<float> Parameters : register(t0);
Texture2D<float3> Image : register(t1);

RWStructuredBuffer<int> Gradients : register(u0);

float int_to_float(int value) {
	return value / (float)(1 << 10);
}

int float_to_int(float value) {
	return (int)(value * (1 << 10));
}

#define MAJORANT_OFFSET 0
#define ALBEDO_OFFSET 1
#define GFACTOR_OFFSET 2
#define DENSITY_OFFSET 3

#define GRID_SIZE 256

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

// Random using is HybridTaus
#include "..\Tools\Randoms.h"
#include "..\Tools\CommonEnvironment.h"

// SCATTERING PHASE FUNCTION

#include "..\Tools\HGPhaseFunction.h"
//#include "..\Tools\MiePhase.h"

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
	maxim = float3(1, 1, 1) * 0.5;
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

	return Parameters[DENSITY_OFFSET + index];
}

float BoxExit(float3 bMin, float3 bMax, float3 x, float3 w)
{
	float2x3 C = float2x3(bMin - x, bMax - x);
	float2x3 D2 = float2x3(w, w);
	float2x3 T = abs(D2) <= 0.000001 ? float2x3(float3(-1000, -1000, -1000), float3(1000, 1000, 1000)) : C / D2;
	return min(min(max(T._m00, T._m10), max(T._m01, T._m11)), max(T._m02, T._m12));
}

void forward_PTStep(inout float T, inout float3 x, inout float3 w, inout float3 B, float xi1, float xi2, float xi3, float xi4, out float t, out bool scatter) {
	float majorant = Parameters[MAJORANT_OFFSET];
	float g = Parameters[GFACTOR_OFFSET];
	float albedo = Parameters[ALBEDO_OFFSET];

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

float EvalPhaseDerivative(float g, float c)
{
	return 0.25 / pi * (
		2 * g / pow(-2 * c * g + g * g + 1, 1.5) -
		3 * (1 - g * g) * (g - c) / pow(-2 * c * g + g * g + 1, 2.5)
		);
}

float // return dL/dTi = dL/dTo * dTo/dTi
	backward_PTStep(
	inout float T, inout float3 x, inout float3 w, inout float3 B,  // Path states
	float xi1, float xi2, float xi3, float xi4, // randoms
	float dL_dTo // backprop differential to compute gradients
	)
{
	float majorant = Parameters[MAJORANT_OFFSET];
	float g = Parameters[GFACTOR_OFFSET];
	float albedo = Parameters[ALBEDO_OFFSET];

	float3 xo = x;
	float3 wo = w;
	float To = T;
	float3 Bo = B;
	float sigma_xo = SampleGrid(xo) * majorant;
	bool scatter = xi4 < sigma_xo / majorant;
	float3 wi = wo;
	float3 Bi = Bo;
	if (scatter)
		ImportanceSamplePhaseBack(g, wi, Bi, xi2, xi3);
	float t = -log(1 - xi1) / majorant;
	float3 xi = xo - t * wo;
	float Ti = To / (1 + scatter * (albedo - 1));

	T = Ti;
	x = xi;
	w = wi;
	B = Bi;

	float dTo_dsigma = (scatter ? -albedo / sigma_xo : 1.0 / (majorant - sigma_xo)) * Ti;
	//float dTo_dg = scatter ? albedo * To / EvalPhase(g, wi, wo) * EvalPhaseDerivative(g, dot(wi, wo)) : 0;

	int3 xo_voxel = (xo - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);

	/*if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
		Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] +=
		dL_dTo * dTo_dsigma / majorant;*/
	if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
		InterlockedAdd(
			Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x],
			float_to_int(dL_dTo * dTo_dsigma * majorant)
			);

	//Gradients[GFACTOR_OFFSET] += dL_dTo * dTo_dg;

	return dL_dTo * (1 + scatter * (albedo - 1)); // dL/dTi = dL/dTo * dTo/dTi
}

void createOrthoBasis(float3 N, out float3 T, out float3 B)
{
	float s = sign(N.z);
	float a = -1.0f / (s + N.z);
	float b = N.x * N.y * a;
	T = float3(1.0f + s * N.x * N.x * a, s * b, -s * N.x);
	B = float3(b, s + N.y * N.y * a, -N.y);
}

void ForwardAndBackwardPathtrace(float3 x, float3 w, float3 Radiance)
{
	float tMin, tMax;
	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
		return;

	float3 Tang, B;
	createOrthoBasis(w, Tang, B);

	int N = 0;
	float T = 1;
	float d = tMax - tMin;
	x += w * tMin; // start in box border.

	int seed = random() * 10000000;

	// Forward VPT
	while (true) {

		float4 xi = randoms_by_seed(seed);
		seed += seed_increment;

		float t;
		bool scatter;
		forward_PTStep(T, x, w, B, xi.x, xi.y, xi.z, xi.w, t, scatter);
		N++;

		d -= t;

		if (d <= 0) // exit box
			break;

		if (scatter)
			d = BoxExit(Grid_Min, Grid_Max, x, w);
	}

	// estimated radiance
	float3 eR = saturate(T * (SampleSkybox(x, w) + SampleLight(w)));
	// compute Loss L2
	//float L = (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]) ^ 2;

	float dL_dT = 2 * (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]);// saturate(SampleSkybox(x, w) + SampleLight(w))[NumberOfPasses % 3];
		//-sign(Radiance[NumberOfPasses % 3] - eR[NumberOfPasses % 3]) * (SampleSkybox(x, w) + SampleLight(w))[NumberOfPasses % 3];

	for (int i = 0; i < N; i++)
	{
		seed -= seed_increment;
		float4 xi = randoms_by_seed(seed);

		dL_dT = backward_PTStep(
			T, x, w, B,  // Path states
			xi.x, xi.y, xi.z, xi.w, // randoms
			dL_dT // backprop differential to compute gradients
		);
	}

	//return lerp(float3(1, 0, 0), float3(0, 1, 0), pdf * 0.5);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim;
	Image.GetDimensions(dim.x, dim.y);

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

	ForwardAndBackwardPathtrace(O, D, Image[uint2(DTid.x, dim.y - DTid.y)]);
}
