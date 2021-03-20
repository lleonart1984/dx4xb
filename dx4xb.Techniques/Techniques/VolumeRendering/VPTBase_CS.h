/// Common header to any compute shader that will use the volume in a scene retained as follow.

#include "..\Tools\Definitions.h"

Texture3D<float3> Grid : register(t0, space0); // Grid used in this pathtracer (value, totalSum, totalSqrSum)

sampler VolSampler : register(s0);

cbuffer Camera : register(b0) {
	float4x4 FromProjectionToWorld;
};

// CB used to count passes of the PT and to know if the output image is a complexity image or the final color.
cbuffer AccumulativeInfo : register(b1) {
	uint NumberOfPasses;
	uint ShowComplexity;
	float PathtracingRatio;
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
	if (tMax < tMin || tMax < 0) {
		return false;
	}
	return true;
}

float SampleGrid(float3 P) {
	float3 minim, maxim;
	GetGridBox(minim, maxim);
	float3 coordinate = (P - minim) / (maxim - minim);
	return Grid.SampleGrad(VolSampler, coordinate, 0, 0);
}

float2 SampleStatistics(float3 P, float r, float3 minim, float3 maxim)
{
	int w, h, d;
	Grid.GetDimensions(w, h, d);
	float3 cellCorrection = 0.5f / float3(w, h, d);

	float3 a = max(minim, P - r + cellCorrection);
	float3 b = min(maxim, P + r + cellCorrection);

	float3 cmin = (a - minim) / (maxim - minim);
	float3 cmax = (b - minim) / (maxim - minim);

	float2 samp000 = Grid.SampleGrad(VolSampler, float3(cmin.x, cmin.y, cmin.z), 0, 0, 0).yz;
	float2 samp001 = Grid.SampleGrad(VolSampler, float3(cmin.x, cmin.y, cmax.z), 0, 0, 0).yz;
	float2 samp010 = Grid.SampleGrad(VolSampler, float3(cmin.x, cmax.y, cmin.z), 0, 0, 0).yz;
	float2 samp011 = Grid.SampleGrad(VolSampler, float3(cmin.x, cmax.y, cmax.z), 0, 0, 0).yz;
	float2 samp100 = Grid.SampleGrad(VolSampler, float3(cmax.x, cmin.y, cmin.z), 0, 0, 0).yz;
	float2 samp101 = Grid.SampleGrad(VolSampler, float3(cmax.x, cmin.y, cmax.z), 0, 0, 0).yz;
	float2 samp110 = Grid.SampleGrad(VolSampler, float3(cmax.x, cmax.y, cmin.z), 0, 0, 0).yz;
	float2 samp111 = Grid.SampleGrad(VolSampler, float3(cmax.x, cmax.y, cmax.z), 0, 0, 0).yz;

	return (samp111 - samp110 - samp101 - samp011 + samp100 + samp010 + samp001 - samp000);// / ((cmax.x - cmin.x) * (cmax.y - cmin.y) * (cmax.z - cmin.z));
}

float3 Pathtrace(float3 x, float3 w, out int counter);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim;
	Output.GetDimensions(dim.x, dim.y);

	StartRandomSeedForRay(dim, 1, DTid.xy, 0, NumberOfPasses);

	float4 ndcP = float4(2 * (DTid.xy + 0.5) / dim - 1, 0, 1);
	ndcP.y *= -1;
	float4 ndcT = ndcP + float4(0, 0, 1, 0);

	float4 viewP = mul(ndcP, FromProjectionToWorld);
	viewP.xyz /= viewP.w;
	float4 viewT = mul(ndcT, FromProjectionToWorld);
	viewT.xyz /= viewT.w;

	float3 O = viewP.xyz;
	float3 D = normalize(viewT.xyz - viewP.xyz);

	int counter = 0;
	float3 radiance = Pathtrace(O, D, counter);

	AccumulateOutput(DTid.xy, radiance, counter);
}
