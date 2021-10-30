#include "..\Tools\Parameters.h"

// This includes vertex info and materials in space1, as well as acummulation buffers for a path-tracer
#include "..\Tools\CommonPT.h" 

// Random using is HybridTaus
#include "..\Tools\Randoms.h"

// Includes some functions for surface scattering and texture mapping
#include "..\Tools\Scattering.h"

// Top level structure with the scene
RaytracingAccelerationStructure Scene : register(t0, space0);

cbuffer Lighting : register(b0) {
	float3 LightPosition;
	float3 LightIntensity;
	float3 LightDirection;
}

#include "../Tools/CommonEnvironment.h"

// CB used to transform rays from projection space to world.
cbuffer Transforming : register(b1) {
	float4x4 FromProjectionToWorld;
}

#include "../Tools/HGPhaseFunction.h"

struct RayPayload // Only used for raycasting
{
	int TriangleIndex;
	int VertexOffset;
	int MaterialIndex;
	int TransformIndex;
	float3 Barycentric;
};

struct RayHitAttribute {
	float3 barycentrics; // Extended to be used also for volume intersections
};

/// Use RTX TraceRay to detect a single intersection. No recursion is necessary
/// Path-tracing is implemented iteratively, no recursively.
bool Intersect(float3 P, float3 D, out RayPayload payload) {
	payload = (RayPayload)0;
	RayDesc ray;
	ray.Origin = P;
	ray.Direction = D;
	ray.TMin = 0;
	ray.TMax = 100.0;
	TraceRay(Scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, ray, payload);
	return payload.TriangleIndex >= 0;
}

// Represents a single bounce of path tracing
// Will accumulate emissive and direct lighting modulated by the carrying importance
// Will update importance with scattered ratio divided by pdf
// Will output scattered ray to continue with
void SurfelScattering(inout float3 x, inout float3 w, inout float3 importance, Vertex surfel, Material material)
{
	float3 V = -w;

	float NdotV;
	bool invertNormal;
	float3 fN;
	float4 R, T;
	ComputeImpulses(V, surfel, material,
		NdotV,
		invertNormal,
		fN,
		R,
		T);

	float3 ratio;
	float3 direction;
	float pdf;
	RandomScatterRay(V, fN, R, T, material, ratio, direction, pdf);

	// Update gathered Importance to the viewer
	importance *= max(0, ratio);
	// Update scattered ray
	w = direction;
	x = surfel.P + sign(dot(direction, fN)) * 0.0001 * fN;
}

float3 ComputePath(float3 O, float3 D, inout int complexity);

[shader("anyhit")]
void CheckMaskOnHit(inout RayPayload payload, in RayHitAttribute attr)
{
	payload.Barycentric = float3(1 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);
	GetIndices(payload.TransformIndex, payload.MaterialIndex, payload.TriangleIndex, payload.VertexOffset);

	Vertex surfel = (Vertex)0;
	Material material = (Material)0;
	VolumeMaterial volMaterial = (VolumeMaterial)0;
	GetHitInfo(
		payload.Barycentric,
		payload.MaterialIndex,
		payload.TriangleIndex,
		payload.VertexOffset,
		payload.TransformIndex,
		surfel, material, volMaterial, 0, 0);

	if (material.RefractionIndex == 0) // transparent
		IgnoreHit();
}


bool IntersectSphere(float3 x, float3 w, out float dist)
{
	//float a = dot(w,w); <- 1 because w is normalized
	float b = 2 * dot(x, w);
	float c = dot(x, x) - 0.05;

	float Disc = b * b - 4 * c;

	if (Disc < 0)
	{
		dist = 0;
		return false;
	}

	// Assuming x is inside the sphere, only the positive root is needed (intersection forward w).
	dist = (-b - sqrt(Disc)) / 2;
	return true;
}

// Gets the Volume Box in local coordinates
void GetVolumeBox(Texture3D<float> grid, out float3 minim, out float3 maxim) {
	int width, height, depth;
	grid.GetDimensions(width, height, depth);

	float maxC = max(width, max(height, depth));
	maxim = float3(width, height, depth) * 0.5 / maxC;
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

bool IntersectVolume(int gridID, int materialIndex, float3 x, float3 w, out float tValue, out float3 coordinates) {

	Texture3D<float> grid = Grids[gridID];

	VolumeMaterial volMaterial = VolMaterials[materialIndex];

	tValue = 0;
	coordinates = x;

	float density = volMaterial.Extinction[NumberOfPasses % 3];

	float3 bMin, bMax;
	GetVolumeBox(grid, bMin, bMax);

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return false;

	coordinates = x + w * tMin; // move free to box border...
	tValue = tMin;

	float d = tMax - tMin;

	while (true) {
		float t = -log(max(0.000000001, 1 - random())) / density;
		
		coordinates += w * min(t, d);
		tValue += t;

		if (t >= d)
			return false;

		d -= t;

		float3 tSamplePosition = (coordinates - bMin) / (bMax - bMin);

		//float scatteringAlbedo = length(tSamplePosition - 0.5) < 0.5;
		float scatteringAlbedo = grid.SampleGrad(VolumeSampler, tSamplePosition, 0, 0);

		if (random() < scatteringAlbedo)
			return true;
	}
}

[shader("intersection")]
void DeltaTracking()
{
	// Attaches current shader to a specific random sequence
	AttachToRandomSequence(DispatchRaysIndex().xy);

	RayHitAttribute attr;

	int volumeID = ObjectInfo.StartTriangle;

	float3 x = ObjectRayOrigin();
	float3 w = normalize(ObjectRayDirection());

	float3 coordinates = 0;
	float t;
	if (IntersectVolume(volumeID, ObjectInfo.MaterialIndex, x, w, t, coordinates))
	{
		attr.barycentrics = coordinates;
		ReportHit(t / length(ObjectRayDirection()), 0, attr);
	}
}

//[shader("anyhit")]
//void DeltaTrackingAnyHit(inout RayPayload payload, in RayHitAttribute attr) {
//	//AcceptHitAndEndSearch();
//}

[shader("closesthit")]
void OnClosestHit(inout RayPayload payload, in RayHitAttribute attr)
{
	GetIndices(payload.TransformIndex, payload.MaterialIndex, payload.TriangleIndex, payload.VertexOffset);
	payload.Barycentric = attr.barycentrics;
}

[shader("miss")]
void OnMiss(inout RayPayload payload)
{
	payload.TriangleIndex = -1;
}

[shader("raygeneration")]
void RayGen()
{
	uint2 raysIndex = DispatchRaysIndex().xy;

	uint2 raysDimensions = DispatchRaysDimensions().xy;
	StartRandomSeedForRay(raysDimensions, 1, raysIndex, 0, NumberOfPasses + (AnimatedFrame ^ 37)*1000000);

	float2 coord = (raysIndex.xy + float2(random(), random())) / raysDimensions;

	float4 ndcP = float4(2 * coord - 1, 0, 1);
	ndcP.y *= -1;
	float4 ndcT = ndcP + float4(0, 0, 1, 0);

	float4 viewP = mul(ndcP, FromProjectionToWorld);
	viewP.xyz /= viewP.w;
	float4 viewT = mul(ndcT, FromProjectionToWorld);
	viewT.xyz /= viewT.w;

	float3 O = viewP.xyz;
	float3 D = normalize(viewT.xyz - viewP.xyz);

	int complexity = 0;

	float3 color = ComputePath(O, D, complexity);

	if (any(isnan(color)))
		color = float3(0, 0, 0);

	AccumulateOutput(raysIndex, color, complexity);
}
