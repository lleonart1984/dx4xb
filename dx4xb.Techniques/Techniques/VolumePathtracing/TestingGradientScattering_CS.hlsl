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

cbuffer VolumeMaterial : register(b2) {
	float3 Extinction; float f0;
	float3 ScatteringAlbedo; float f1;
	float3 G; float f2;
};

cbuffer Lighting : register(b3) {
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

void GetGridBox(out float3 minim, out float3 maxim) {
	maxim = float3(1,1,1);
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
	if (tMax <= tMin || tMax <= 0) {
		return false;
	}
	return true;
}


static int complexity;
static int scatters;

static float3 Grid_Min, Grid_Max;

#define GRADIENT (float3(0.0, 1.0, 0))


float GetComponent(float3 radiance) {
	return radiance[NumberOfPasses % 3];
}


float GetDensity(float3 P) {
	if (length(P) >= 1)
		return 0;
	return (dot(P, GRADIENT) + 1) * GetComponent(Extinction);
}

float GetMajorant(float3 x, float r) {
	float centerDensity = GetDensity(x);
	return (r + 1) * centerDensity;
}

float IntersectSphere(float3 x, float3 w)
{
	float xDotW = dot(x, w);
	float xDotX = dot(x, x);
	return sqrt(max(0, xDotW * xDotW - xDotX + 1)) - xDotW;
}

void UnitarySphereTransmittance(float density, float3 gradient, out float3 x, out float3 w) {
	x = 0;
	w = float3(0, 0, 1);
	float majorant = density * (1 + length(gradient));
	float d = IntersectSphere(x, w);

	while (true) {
		float t = -log(1 - random()) / majorant;

		if (t >= d)
		{
			x += w * d;
			return;
		}

		x += w * t;
		
		float currentDensity = density * (1 + dot(x, gradient));
		if (random() < currentDensity / majorant)
		{
			//if (random() >= GetComponent(ScatteringAlbedo))
			//	return 0;
			w = ImportanceSamplePhase(GetComponent(G), w); // scattering event...
			d = IntersectSphere(x, w);
		}
		else
			d -= t;
	}
}

#include "LinearMultiscatteringModels.h"

void ModelUnitarySphereTransmittance(float density, float3 gradient, out float3 x, out float3 w) {
	GenerateNAPathWithModel(density, gradient, GetComponent(G), x, w);
	//x = normalize(x);
	//w = normalize(w);
	//return 1;
}


/// <summary>
/// Computes a transmittance computation through a sphere volume starting flying in the center x in direction w.
/// Returns the transmittance probability, x and w are updated to the exiting position
/// </summary>
void SphereTransmittance(inout float3 x, inout float3 w, float tMax)
{
	float r = 1 - length(x);
	float majorant = GetMajorant(x, r);

	if (r * majorant > 30 * (1 + length(GRADIENT)))
	{
		r = 30 * (1 + length(GRADIENT)) / majorant;
		majorant = GetMajorant(x, r);
	}

	if (r * majorant < 2)
	{
		majorant = 2 * GetComponent(Extinction);
		float t = -log(1 - random()) / majorant;

		if (t > tMax)
		{
			x += w * tMax;
			return;
		}

		x += w * t;

		float density = GetDensity(x);

		if (random() < density / majorant)
		{
			//if (random() < 1 - GetComponent(ScatteringAlbedo))
			//	return 0;

			w = ImportanceSamplePhase(GetComponent(G), w); // scattering event...
		}
	}
	else {
		float3 xc = x;
		float3 win = w;

		float3 temp = abs(win.x) >= 0.9999 ? float3(0, 0, 1) : float3(1, 0, 0);
		float3 winY = normalize(cross(temp, win));
		float3 winX = cross(win, winY);
		float3x3 R = float3x3(winX, winY, win);
		
		// Normalize to unit sphere
		float3 gradient = mul(GRADIENT, transpose(R)) * r;
		//float3 gradient = GRADIENT * r;
		float density = GetDensity(x) * r;

		// Check for free-flight
		float a = density;
		float b = a * (1 + gradient.z);
		float LT = exp(-(a - (a - b) * 0.5));
		
		//UnitarySphereTransmittance(density, gradient, x, w);

		if (random() < LT) // No scattering in linear media
		{
			x += w * r;
		}
		else {
			ModelUnitarySphereTransmittance(density, gradient, x, w);

			x = mul(x, R);
			w = mul(w, R);

			x = xc + x * r;
			//return T;
		}
	}
}

void PTSphereTransmittance(inout float3 x, inout float3 w, float tMax)
{
	float majorant = 2 * GetComponent(Extinction);

	float t = -log(1 - random()) / majorant;

	if (t > tMax)
	{
		x += w * tMax;
		return;
	}

	x += w * t;

	float density = GetDensity(x);

	if (random() < density / majorant)
	{
		/*if (random() < 1 - GetComponent(ScatteringAlbedo))
			return 0;*/

		w = ImportanceSamplePhase(GetComponent(G), w); // scattering event...
	}

	//return 1;
}

float Pathtrace(float3 x, float3 w, bool useAcceleration)
{
	while (true) {

		complexity++;

		float tMin, tMax;
		if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
#ifdef NO_DRAW_LIGHT_SOURCE
			return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#else
			return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#endif

		x += w * tMin;

		if (useAcceleration)
			SphereTransmittance(x, w, tMax);
		else
			PTSphereTransmittance(x, w, tMax);
	}
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim;
	Output.GetDimensions(dim.x, dim.y);

	StartRandomSeedForRay(dim, 1, DTid.xy, 0, NumberOfPasses + (AnimatedFrame ^ 37) * 1000000);

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

	float3 radiance = 0;
	float value = Pathtrace(O, D, DTid.x / (float)dim.x > 0.5);
	radiance[NumberOfPasses % 3] = 3 * value;

	AccumulateOutput(DTid.xy, radiance, complexity);
}


