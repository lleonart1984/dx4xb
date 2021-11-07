#include "..\Tools\Definitions.h"

struct PTResult {
	float3 Origin;
	float3 Direction;
	float3 Radiance;
};

StructuredBuffer<float> Parameters : register(t0);
StructuredBuffer<PTResult> Paths   : register(t1);

RWStructuredBuffer<uint> Gradients : register(u0);

void SafeIncrementGradient(int pos, float increment) {
	if (increment == 0)
		return;
	//Gradients[pos] = asuint(asfloat(Gradients[pos]) + increment);
	uint old = Gradients[pos];
	uint assumed = old + 1;
	[allow_uav_condition]
	while (assumed != old)
	{
		assumed = Gradients[pos];
		InterlockedCompareExchange(Gradients[pos], assumed, asuint(asfloat(assumed) + increment), old);
	}
}


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

// Random using is HybridTaus
#include "..\Tools\RandomsLight.h"
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

float SampleGrid(int3 coord) {
	int index = coord.z * (GRID_SIZE * GRID_SIZE) + coord.y * GRID_SIZE + coord.x;
	return saturate(Parameters[DENSITY_OFFSET + index]);
}

float SampleGrid(float3 P) {
	complexity++;
	float3 coordinate = (P - Grid_Min) / (Grid_Max - Grid_Min);

	int3 coord = coordinate * GRID_SIZE;

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

void forward_Transmittance(inout float log_T, inout float3 x, inout float3 w, out float t)
{
	float majorant = 100;// Parameters[MAJORANT_OFFSET];
	t = 0.005;
	x += w * t;
	float sigma_xo = SampleGrid(x) * majorant;
	log_T += (-sigma_xo * t);
}

//float backward_Transmittance(inout float log_T, inout float3 x, inout float3 w, float dL_dTo) {
//	float majorant = 100;// Parameters[MAJORANT_OFFSET];
//	float dx = 0.005;
//	float3 xo = x;
//	float sigma_xo = SampleGrid(x) * majorant;
//	float old_log_T = log_T;
//	log_T -= (-sigma_xo * dx);
//	x -= w * dx;
//	float dTo_dsigma = -exp(old_log_T) * dx;
//	float dTo_dTi = exp(-sigma_xo * dx);
//
//	int3 xo_voxel = (xo - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);
//
//	/*if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
//		Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] +=
//		dL_dTo * dTo_dsigma / majorant;*/
//	if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
//		InterlockedAdd(
//			Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x],
//			float_to_int(dL_dTo * dTo_dsigma * majorant)
//		);
//		
//			//Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] +=
//			//dL_dTo * dTo_dsigma * majorant;
//
//	//Gradients[GFACTOR_OFFSET] += dL_dTo * dTo_dg;
//
//	return dL_dTo * dTo_dTi; // dL/dTi = dL/dTo * dTo/dTi
//}
//
//void forward_PTStep(inout float T, inout float3 x, inout float3 w, inout float3 B, float xi1, float xi2, float xi3, float xi4, out float t, out bool scatter) {
//	float majorant = 100;// Parameters[MAJORANT_OFFSET];
//	float g = 0.875 * 0.6;// Parameters[GFACTOR_OFFSET];
//	float albedo = 1.0f;// Parameters[ALBEDO_OFFSET];
//
//	float3 xi = x;
//	float3 wi = w;
//	float3 Bi = B;
//	float Ti = T;
//
//	t = -log(1 - xi1) / majorant;
//	float3 xo = xi + t * wi;
//
//	float sigma_xo = SampleGrid(xo) * majorant;
//
//	scatter = xi4 < sigma_xo / majorant;
//	float3 wo = wi;
//	float3 Bo = Bi;
//
//	if (scatter)
//		ImportanceSamplePhase(g, wo, Bo, xi2, xi3);
//
//	float To = (1 + scatter * (albedo - 1)) * Ti;
//
//	T = To;
//	x = xo;
//	w = wo;
//	B = Bo;
//}
//
//float EvalPhaseDerivative(float g, float c)
//{
//	return 0.25 / pi * (
//		2 * g / pow(-2 * c * g + g * g + 1, 1.5) -
//		3 * (1 - g * g) * (g - c) / pow(-2 * c * g + g * g + 1, 2.5)
//		);
//}
//
//float // return dL/dTi = dL/dTo * dTo/dTi
//	backward_PTStep(
//	inout float T, inout float3 x, inout float3 w, inout float3 B,  // Path states
//	float xi1, float xi2, float xi3, float xi4, // randoms
//	float dL_dTo // backprop differential to compute gradients
//	)
//{
//	float majorant = 100;// Parameters[MAJORANT_OFFSET];
//	float g = 0.875 * 0.6;// Parameters[GFACTOR_OFFSET];
//	float albedo = 1.0;// Parameters[ALBEDO_OFFSET];
//
//	float3 xo = x;
//	float3 wo = w;
//	float To = T;
//	float3 Bo = B;
//	float sigma_xo = SampleGrid(xo) * majorant;
//	bool scatter = xi4 < sigma_xo / majorant;
//	float3 wi = wo;
//	float3 Bi = Bo;
//	if (scatter)
//		ImportanceSamplePhaseBack(g, wi, Bi, xi2, xi3);
//	float t = -log(1 - xi1) / majorant;
//	float3 xi = xo - t * wo;
//	float Ti = To / (1 + scatter * (albedo - 1));
//
//	T = Ti;
//	x = xi;
//	w = wi;
//	B = Bi;
//
//	float dTo_dsigma = (scatter ? -albedo / sigma_xo : 1.0 / (majorant - sigma_xo)) * Ti;
//	//float dTo_dg = scatter ? albedo * To / EvalPhase(g, wi, wo) * EvalPhaseDerivative(g, dot(wi, wo)) : 0;
//
//	int3 xo_voxel = (xo - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);
//
//	/*if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
//		Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] +=
//		dL_dTo * dTo_dsigma / majorant;*/
//	if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
//		
//			Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] +=
//			float_to_int(dL_dTo * dTo_dsigma * majorant)
//			;
//
//	//Gradients[GFACTOR_OFFSET] += dL_dTo * dTo_dg;
//
//	return dL_dTo * (1 + scatter * (albedo - 1)); // dL/dTi = dL/dTo * dTo/dTi
//}
//
//void createOrthoBasis(float3 N, out float3 T, out float3 B)
//{
//	float s = sign(N.z);
//	float a = -1.0f / (s + N.z);
//	float b = N.x * N.y * a;
//	T = float3(1.0f + s * N.x * N.x * a, s * b, -s * N.x);
//	B = float3(b, s + N.y * N.y * a, -N.y);
//}

//void ForwardAndBackwardPathtrace(float3 x, float3 w, float3 Radiance)
//{
//	float tMin, tMax;
//	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
//		return;
//
//	float3 Tang, B;
//	createOrthoBasis(w, Tang, B);
//
//	int N = 0;
//	float T = 1;
//	float d = tMax - tMin;
//	x += w * tMin; // start in box border.
//
//	int seed = random() * 10000000;
//
//	// Forward VPT
//	while (true) {
//
//		float4 xi = randoms_by_seed(seed);
//		seed += seed_increment;
//
//		float t;
//		bool scatter;
//		forward_PTStep(T, x, w, B, xi.x, xi.y, xi.z, xi.w, t, scatter);
//		N++;
//
//		d -= t;
//
//		if (d <= 0) // exit box
//			break;
//
//		if (scatter)
//			d = BoxExit(Grid_Min, Grid_Max, x, w);
//	}
//
//	// estimated radiance
//	float3 eR = saturate(T * (SampleSkybox(x, w) + SampleLight(w)));
//	// compute Loss L2
//	//float L = (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]) ^ 2;
//
//	float dL_dT = 2 * (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]);// saturate(SampleSkybox(x, w) + SampleLight(w))[NumberOfPasses % 3];
//		//-sign(Radiance[NumberOfPasses % 3] - eR[NumberOfPasses % 3]) * (SampleSkybox(x, w) + SampleLight(w))[NumberOfPasses % 3];
//
//	for (int i = 0; i < N; i++)
//	{
//		seed -= seed_increment;
//		float4 xi = randoms_by_seed(seed);
//
//		dL_dT = backward_PTStep(
//			T, x, w, B,  // Path states
//			xi.x, xi.y, xi.z, xi.w, // randoms
//			dL_dT // backprop differential to compute gradients
//		);
//	}
//
//	//return lerp(float3(1, 0, 0), float3(0, 1, 0), pdf * 0.5);
//}

//void ForwardAndBackwardTransmittance2(float3 x, float3 w, float3 Radiance)
//{
//	float tMin, tMax;
//	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
//		return;
//
//	int N = 0;
//	float log_T = 0;
//	float d = tMax - tMin;
//	x += w * tMin; // start in box border.
//
//	// Forward VPT
//	while (d > 0) {
//		float t;
//		forward_Transmittance(log_T, x, w, t);
//		N++;
//		d -= t;
//	}
//
//	// estimated radiance
//	float3 eR = exp(log_T) * SampleSkybox(x, w);
//	// compute Loss L2
//	//float L = (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]) ^ 2;
//
//	float dL_dT = (2 * (eR - Radiance) * SampleSkybox(x, w))[NumberOfPasses % 3];
//
//	for (int i = 0; i < N; i++)
//	{
//		dL_dT = backward_Transmittance(
//			log_T, x, w, 
//			dL_dT // backprop differential to compute gradients
//		);
//	}
//
//	//return lerp(float3(1, 0, 0), float3(0, 1, 0), pdf * 0.5);
//}
//
//
//void ForwardAndBackwardTransmittance3(float3 x, float3 w, float3 Radiance)
//{
//	float tMin, tMax;
//	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
//		return;
//
//	int N = 0;
//	float tau = 0;
//	float d = tMax - tMin;
//	x += w * tMin; // start in box border.
//
//	float majorant = 100;// Parameters[MAJORANT_OFFSET];
//	float dt = 0.005;
//	// Compute Transmittance
//	for (float t = 0; t < d; t += dt) {
//		float sigma_xo = SampleGrid(x + w * t) * majorant;
//		tau += sigma_xo * dt;
//	}
//
//	float T = exp(-tau);
//
//	float dT_dsigma = -T * dt;
//
//	float3 eR = T * SampleSkybox(x + w * d, w);
//	// compute Loss L2
//	//float L = (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]) ^ 2;
//	float dL_dT = sign(eR - Radiance)[NumberOfPasses % 3];// (2 * (eR - Radiance) * SampleSkybox(x, w))[NumberOfPasses % 3];
//
//	// Compute Derivative for each sigma_xo
//	for (float t = 0; t < d; t += dt) {
//		float3 xo = x + w * t;
//		int3 xo_voxel = (xo - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);
//
//		if (all(xo_voxel >= 0) && all(xo_voxel < GRID_SIZE))
//			/*InterlockedAdd(
//				Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x],
//				(dL_dT * dT_dsigma)
//			);*/
//			//Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] += float_to_int(dL_dT * dT_dsigma * majorant);
//			Gradients[DENSITY_OFFSET + xo_voxel.z * GRID_SIZE * GRID_SIZE + xo_voxel.y * GRID_SIZE + xo_voxel.x] 
//			+= (dL_dT * dT_dsigma * majorant);
//	}
//}


void ForwardAndBackwardTransmittance(float3 x, float3 w, float3 Radiance)
{
	float tMin, tMax;
	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
		return;
	float d = tMax - tMin;
	x += w * tMin; // start in box border.

	float majorant = 100;

	float tau = 0;
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

	float T = exp(-tao);

	float3 eR = T * SampleSkybox(x + w * d, w);
	// compute Loss L2
	//float L = (eR[NumberOfPasses % 3] - Radiance[NumberOfPasses % 3]) ^ 2;
	float dL_dT = sign(eR - Radiance)[NumberOfPasses % 3];// (2 * (eR - Radiance) * SampleSkybox(x, w))[NumberOfPasses % 3];
	//float dL_dT = (2 * (eR - Radiance) * SampleSkybox(x, w))[NumberOfPasses % 3];

	v = (x - Grid_Min) * GRID_SIZE / (Grid_Max - Grid_Min);
	alpha = abs(corner - x) / max(0.000001, abs(w));

	current_t = 0;

	while (current_t < d) {
		float next_t = min(alpha.x, min(alpha.y, alpha.z));
		int3 v_inc = int3(
			alpha.x <= alpha.y && alpha.x <= alpha.z,
			alpha.x > alpha.y && alpha.y <= alpha.z,
			alpha.x > alpha.z && alpha.y > alpha.z);
		float dt = next_t - current_t;

		float dT_dsigma = -T * dt;

		if (all(v >= 0) && all(v < GRID_SIZE))
			SafeIncrementGradient(v.z * GRID_SIZE * GRID_SIZE + v.y * GRID_SIZE + v.x, 
				dL_dT * dT_dsigma * majorant);

		alpha += v_inc * alpha_inc;
		v += v_inc * (w >= 0 ? 1 : -1);
		current_t = next_t;
	}
}


[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint length, width;
	Paths.GetDimensions(length, width);

	if (DTid.x >= length)
		return;

	StartRandomSeed(DTid.x + NumberOfPasses * length);

	PTResult pt = Paths[DTid.x];

	float3 O = pt.Origin;
	float3 D = pt.Direction;
	float3 R = pt.Radiance;

	;

	GetGridBox(Grid_Min, Grid_Max);

	//if (all(R >= 0.9))
		ForwardAndBackwardTransmittance(O, normalize(D), R);
}
