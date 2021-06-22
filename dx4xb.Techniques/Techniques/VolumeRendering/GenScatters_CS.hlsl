/// Common header to any compute shader that will use the volume in a scene retained as follow.

#include "..\Tools\Definitions.h"

Texture3D<float> Grid	: register(t0, space0); // Grid used in this pathtracer (value, average, radii)

sampler VolSampler : register(s0);

cbuffer Camera : register(b0) {
	float4x4 FromWorldToProjection;
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
RWTexture2D<uint> Complexity					: register(u0, space0); // Complexity buffer of scattering events

void AccumulateScattering(float3 P) {
	float4 p = mul(float4(P, 1), FromWorldToProjection);
	float3 proj = p.xyz / p.w;
	if (any(proj < -1) || any(proj > 1))
		return;
	int w, h;
	Complexity.GetDimensions(w, h);
	float2 viewport = float2((proj.x * 0.5 + 0.5) * w, (0.5 - proj.y * 0.5) * h);
	InterlockedAdd(Complexity[viewport], 1);
}

// Random using is HybridTaus
#include "..\Tools\Randoms.h"

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

float SampleGrid(float3 P) {
	float3 minim, maxim;
	GetGridBox(minim, maxim);
	float3 coordinate = (P - minim) / (maxim - minim);
	return Grid.SampleGrad(VolSampler, coordinate, 0, 0).x;
}

#include "..\Tools\HGPhaseFunction.h"
//#include "..\Tools\MiePhase.h"

#include "..\Tools\PerlinNoise.h"

void DTPathtrace(float3 x, float3 w) {
	int component = random() * 3;
	float density = Extinction[component]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return;
	
	float d = tMax - tMin;
	x += w * tMin;

	int scatters = 0;

	while (true) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / density; // majorant of all volume data

		if (t >= d)
			return;

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float probExt = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x;

		if (probExt == 0.5) {
			probExt = saturate(snoise(tSamplePosition * 10) * 0.5 + 0.5 + snoise(tSamplePosition * 21) * 0.4 + snoise(tSamplePosition * 47) * 0.2);
		}

		float m_t = probExt * density; // extinction coef
		float m_s = m_t * ScatteringAlbedo[component]; // scattering coef
		float m_a = m_t - m_s; // absorption coef
		float m_n = density - m_t; // null coef

		float xi = random();

		float Pa = m_a / density;
		float Ps = m_s / density;
		float Pn = m_n / density;

		//AccumulateScattering(x);
		if (xi < Pa) // absorption
			return;

		if (xi < 1 - Pn) // scattering
		{
			if (abs(x.x) < 0.01)
				AccumulateScattering(x);

			w = ImportanceSamplePhase(G[component], w); // scattering event...

			if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
				return;

			d = tMax - tMin;
			x += w * tMin;
		}
		else {
			//if (abs(x.z) < 0.01)
			//	AccumulateScattering(x);
			// if no absorption and no scattering null collision occurred
			d -= t;
		}
	}
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim = uint2(1024, 1024); // dispatched paths

	StartRandomSeedForRay(dim, 1, DTid.xy, 0, NumberOfPasses + (AnimatedFrame ^ 37) * 1000000);

	float3 minim, maxim;
	GetGridBox(minim, maxim);
	float areaXY = (maxim.x - minim.x) * (maxim.y - minim.y);
	float areaXZ = (maxim.x - minim.x) * (maxim.z - minim.z);
	float areaYZ = (maxim.z - minim.z) * (maxim.y - minim.y);
	float sideAreas = areaXY + areaXZ + areaYZ;
	float side = random()*sideAreas;

	float2 ruv = int2(float2(random(), random())*10)/10.0;
	float3 P;
	if (side < areaXY)
		P = float3(ruv.x * (maxim.x - minim.x) + minim.x, ruv.y * (maxim.y - minim.y) + minim.y, LightDirection.z < 0 ? minim.z : maxim.z);
	else
		if (side < areaXY + areaXZ)
			P = float3(ruv.x * (maxim.x - minim.x) + minim.x, LightDirection.y < 0 ? minim.y : maxim.y, ruv.y* (maxim.z - minim.z) + minim.z);
		else
			P = float3(LightDirection.x < 0 ? minim.x : maxim.x, ruv.y* (maxim.y - minim.y) + minim.y, ruv.x* (maxim.z - minim.z) + minim.z);

	P = LightDirection * 2;
	float3 D = -LightDirection;

	DTPathtrace(P, D);
}
