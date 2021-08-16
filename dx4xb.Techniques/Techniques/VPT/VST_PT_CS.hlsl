/// Common header for Volumetric Pathtracers.

#include "..\Tools\Definitions.h"

Texture3D<float> Grid			: register(t0); // Grid used in this pathtracer (average density and radius)
Texture3D<float4> Parameters	: register(t1); 
Texture3D<int> Radii			: register(t2); // Optimal radius

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
static int scatters;

static float3 Grid_Min, Grid_Max;

float SampleGrid(float3 P) {
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);
	float3 coordinate = (P - Grid_Min) * dim / (Grid_Max - Grid_Min) + float3(random(), random(), random()) - 0.5;
	return Grid[coordinate];
}

void SampleGridParametrization(float3 P, out float3 gradient, out float density, out float radius) {
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);
	float3 coordinate = (P - Grid_Min) * dim / (Grid_Max - Grid_Min) + float3(random(), random(), random()) - 0.5;
	gradient = Parameters[coordinate].xyz;
	density = Parameters[coordinate].w;
	radius = max(1, Radii[coordinate]);

	//gradient = 0;
	//density = Grid[coordinate];
	//radius = 1;
}

float GetComponent(float3 radiance) {
	return radiance[NumberOfPasses % 3];
}

float GridExit(float3 x, float3 w)
{
	float2x3 C = float2x3(Grid_Min - x, Grid_Max - x);
	float2x3 D2 = float2x3(w, w);
	float2x3 T = abs(D2) <= 0.000001 ? float2x3(float3(-1000, -1000, -1000), float3(1000, 1000, 1000)) : C / D2;
	return min(min(max(T._m00, T._m10), max(T._m01, T._m11)), max(T._m02, T._m12));
}

float IntersectSphere(float3 x, float3 w)
{
	float xDotW = dot(x, w);
	float xDotX = dot(x, x);
	return sqrt(xDotW * xDotW - xDotX + 1) - xDotW;
}

float UnitaryHomogeneousSpherePathSample(float density, float albedo, float g, out float3 x, out float3 w)
{
	x = 0;
	w = float3(0, 0, 1);

	float d = 1;

	while (true) {
		float t = -log(1 - random()) / max(0.0000000001, density);
		x += w * min(t, d);
		if (t > d)
			return 1;
		if (random() >= albedo) // absorption
			return 0;
		w = ImportanceSamplePhase(g, w);
		scatters++;
		d = IntersectSphere(x, w);
	}
}
#if 1
float UnitaryLinearSpherePathSample(float3 gradient, float density, float albedo, float g, out float3 x, out float3 w)
{
	x = 0;
	w = float3(0, 0, 1);

	float majorant = density * (1 + length(gradient));

	float d = 1;

	while (true) {
		float t = -log(1 - random()) / max(0.0000000001, majorant);
		x += w * min(t, d);
		if (t > d)
			return 1;
		float sampleD = density * (1 + dot(x, gradient));
		if (random() < sampleD / majorant)
		{
			if (random() >= albedo) // absorption
				return 0;
			w = ImportanceSamplePhase(g, w);
			scatters++;
		}
		d = IntersectSphere(x, w);
	}
}
#else

#include "LinearMultiscatteringModels.h"
float UnitaryLinearSpherePathSample(float3 gradient, float density, float albedo, float g, out float3 x, out float3 w)
{
	if (density < 2) {
		x = 0;
		w = float3(0, 0, 1);

		float majorant = density * (1 + length(gradient));

		float d = 1;

		while (true) {
			float t = -log(1 - random()) / max(0.0000000001, majorant);
			x += w * min(t, d);
			if (t > d)
				return 1;
			float sampleD = density * (1 + dot(x, gradient));
			if (random() < sampleD / majorant)
			{
				if (random() >= albedo) // absorption
					return 0;
				w = ImportanceSamplePhase(g, w);
				scatters++;
			}
			d = IntersectSphere(x, w);
		}
	}
	else {
		x = w = float3(0, 0, 1);
		float a = density;
		float b = a * (1 + gradient.z);
		float LT = exp(-(a - (a - b) * 0.5));
		if (random() >= LT) // No scattering in linear media
			GenerateNAPathWithModel(density, gradient, g, x, w);
		return 1;
	}
}

#endif 

float PTStep(inout float3 x, inout float3 w)
{
	float majorant = GetComponent(Extinction);
	float d = GridExit(x, w);
	float t = -log(1 - random()) / majorant;
	x = x + w * t;
	
	float density = SampleGrid(x);
	//float density = SampleGridViaParametrization(x);

	if (t < d && random() < density) // scatter event
	{
		if (random() < 1 - GetComponent(ScatteringAlbedo)) // absorption
			return 0;
		w = ImportanceSamplePhase(GetComponent(G), w); // scattering event...
		scatters++;
	}

	return 1;
}

float SphereStep(inout float3 x, inout float3 w)
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);
	int maxDim = max(dim.x, max(dim.y, dim.z));

	// Sample density field descriptor at position x
	float3 gradient;
	float density;
	float r;
	SampleGridParametrization(x, gradient, density, r);
	
	r /= (float)maxDim; // from Grid to world

	// Compute transformation matrix from sphere to global coordinates
	float3 temp = abs(w.x) >= 0.9999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 winY = normalize(cross(temp, w));
	float3 winX = cross(w, winY);
	float3x3 fromSphereToGrid = float3x3(winX, winY, w);
	float3x3 fromGridToSphere = transpose(fromSphereToGrid);

	gradient = mul(gradient, fromGridToSphere);


	float3 xs, ws;
	float T = UnitaryLinearSpherePathSample(
		gradient,
		density * GetComponent(Extinction) * r,
		GetComponent(ScatteringAlbedo),
		GetComponent(G), xs, ws);

	scatters += ws.z < 0.999;

	xs = mul(xs, fromSphereToGrid);
	ws = mul(ws, fromSphereToGrid);

	x += xs * r;
	w = ws;

	return T;
}

float3 GetColorForError(float complexity) {

	if (complexity == 0)
		return float3(0, 0, 0);

	//return float3(1,1,1);

	float level = complexity*12;

	float3 stopPoints[13] = {
		float3(0, 0, 128) / 255.0, //1
		float3(49, 54, 149) / 255.0, //2
		float3(69, 117, 180) / 255.0, //4
		float3(116, 173, 209) / 255.0, //8
		float3(171, 217, 233) / 255.0, //16
		float3(224, 243, 248) / 255.0, //32
		float3(255, 255, 191) / 255.0, //64
		float3(254, 224, 144) / 255.0, //128
		float3(253, 174, 97) / 255.0, //256
		float3(244, 109, 67) / 255.0, //512
		float3(215, 48, 39) / 255.0, //1024
		float3(165, 0, 38) / 255.0,  //2048
		float3(200, 0, 175) / 255.0  //4096
	};

	if (level >= 12)
		return stopPoints[12];

	return lerp(stopPoints[(int)level], stopPoints[(int)level + 1], level % 1);
}

//float FirstHit(float3 x, float3 w) {
//	float tMin, tMax;
//	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
//		return GetComponent(SampleSkybox(x, w));
//	
//	float majorant = GetComponent(Extinction);
//	float d = tMax;
//	int avoid = 0;
//	while (true) {
//		float t = -log(1 - random()) / majorant;
//		if (t > d)
//			return GetComponent(SampleSkybox(x, w));
//		d -= t;
//		x += w * t;
//
//		if (x.x < 0 && random() < SampleGrid(x))
//		{
//			if (avoid <= 0)
//			{
//				float3 gradient;
//				float4 parameters;
//				float t;
//				SampleGridParametrization(x, 1, gradient, parameters, t);
//				//if (parameters.x > 0)
//					return GetComponent(GetColorForError(parameters.w));// *4);
//			}
//			else
//				avoid--;
//		}
//	}
//}

float Pathtrace(float3 x, float3 w)
{
	float T = 1;

	while (true) {

		float tMin, tMax;
		if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
			return T * GetComponent(SampleSkybox(x, w) + SampleLight(w));

		x += w * tMin;

		//if (scatters < 5)
		{
			//T *= PTStep(x, w);
			//complexity++;
		}
		//else
		{
		//	int level = min(7, log(max(1, scatters * 0.5 - 5)));
			T *= SphereStep(x, w);
			complexity++;
		}

		if (T < 0.00001)
			return 0;
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
	//float value = FirstHit(O, D);
	float value = Pathtrace(O, D);
	radiance[NumberOfPasses % 3] = 3 * value;

	AccumulateOutput(DTid.xy, radiance, complexity);
}