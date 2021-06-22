/// Common header for Volumetric Pathtracers.

#include "..\Tools\Definitions.h"

Texture3D<float> Grid		: register(t0, space0); // Grid used in this pathtracer density
sampler GridSampler			: register(s0);
cbuffer Camera				: register(b0) {
	float4x4 FromProjectionToWorld;
};
// CB used to count passes of the PT and to know if the output image is a complexity image or the final color.
cbuffer AccumulativeInfo	: register(b1) {
	uint NumberOfPasses;
	uint ShowComplexity;
	float PathtracingRatio;
	uint AnimatedFrame;
}
cbuffer VolumeMaterial		: register(b2) {
	float3 Extinction; float f0;
	float3 ScatteringAlbedo; float f1;
	float3 G; float f2;
};
cbuffer Lighting			: register(b3) {
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

//#include "..\Tools\HGPhaseFunction.h"
#include "..\Tools\MiePhase.h"

void GetGridBox(out float3 minim, out float3 maxim) {
	int w, h, d;
	Grid.GetDimensions(w, h, d);
	float maxDim = max(w, max(h, d));
	maxim = float3(w, h, d) / maxDim * 0.5;
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

float SampleGrid(float3 P) {

	complexity++;

	float3 minim, maxim;
	GetGridBox(minim, maxim);
	float3 coordinate = (P - minim) / (maxim - minim);
	return Grid.SampleGrad(GridSampler, coordinate, 0, 0).x;
}

float GetComponent(float3 radiance) {
	return radiance[NumberOfPasses % 3];
}

/// <summary>
/// When implemented will compute the probability of photon transmission along a ray.
/// </summary>
float Transmittance(float3 x, float3 w, float d, float majorant, out float3 xs);

float BoxExit(float3 bMin, float3 bMax, float3 x, float3 w)
{
	float2x3 C = float2x3(bMin - x, bMax - x);
	float2x3 D2 = float2x3(w, w);
	float2x3 T = abs(D2) <= 0.000001 ? float2x3(float3(-1000, -1000, -1000), float3(1000, 1000, 1000)) : C / D2;
	return min(min(max(T._m00, T._m10), max(T._m01, T._m11)), max(T._m02, T._m12));
}

bool BoxTransmittance(
	float3 bMin, float3 bMax, 
	float majorant, 
	inout float3 x, inout float3 w) {

	float density = GetComponent(Extinction);
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return true;
	x += w * tMin;
	float d = tMax - tMin;

	while (true) {
		float3 xs;
		float T = Transmittance(x, w, d, majorant, xs);

		x = xs;

		if (random() < T) // no more scattering event
			return true;

		if (random() < 1 - albedo) // absorption
			return false; 

		w = ImportanceSamplePhase(g, w); // scattering event...

		d = BoxExit(bMin, bMax, x, w);
	}
}

float Pathtrace(float3 x, float3 w);

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

	complexity = 0;

	float3 radiance = 0;
	float value = Pathtrace(O, D);
	radiance[NumberOfPasses % 3] = 3 * value;

	AccumulateOutput(DTid.xy, radiance, complexity);
}
