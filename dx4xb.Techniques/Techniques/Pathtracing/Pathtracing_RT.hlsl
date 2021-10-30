#include "PathtracingCommon_RT.h"

float3 ComputePath(float3 O, float3 D, inout int complexity)
{
	int cmp = NumberOfPasses % 3;
	float3 importance = 0;
	importance[cmp] = 3;
	float3 x = O;
	float3 w = D;

	bool inMedium = false;

	int bounces = 0;

	// Collect all emissive contribution
	float3 result = 0;

	bool isOutside = true;

	[loop]
	while (true)
	{
		complexity++;

		int tIndex;
		int transformIndex;
		int mIndex;
		float3 coords;

		RayPayload payload = (RayPayload)0;
		if (!Intersect(x, w, payload)) // 
			return result + importance * (SampleSkybox(w) + SampleLight(w) * (bounces > 0));// );

		Vertex surfel = (Vertex)0;
		Material material = (Material)0;
		VolumeMaterial volMaterial = (VolumeMaterial)0;
		bool isSurfaceHit = GetHitInfo(
			payload.Barycentric,
			payload.MaterialIndex,
			payload.TriangleIndex,
			payload.VertexOffset,
			payload.TransformIndex,
			surfel, material, volMaterial, 0, 0);

		//if (!isSurfaceHit)
			//return float3(1,1,0,;
			//return float3(1,0,1);// surfel.P;// float3(1, 1, 0);// surfel.P;

		result += importance * material.Emissive * (1 - material.Roulette.w);

		float d = length(surfel.P - x); // Distance to the hit position.
		float t = !isSurfaceHit || isOutside || volMaterial.Extinction[cmp] == 0 ? 100000000 : -log(max(0.000000000001, 1 - random())) / volMaterial.Extinction[cmp];

		[branch]
		if (t >= d && isSurfaceHit)
		{
			bounces += isOutside;
			if (bounces >= MAX_PATHTRACING_BOUNCES)
				return result;

			SurfelScattering(x, w, importance, surfel, material);

			if (any(material.Specular) && material.Roulette.w > 0)
				isOutside = dot(surfel.N, w) >= 0;
		}
		else
		{ // Volume scattering or absorption in volume or inside geometries
			x += min(t, d) * w; // free traverse in a medium

			if (random() < 1 - volMaterial.ScatteringAlbedo[cmp]) // absorption instead
				return result + importance * material.Emissive;
			
			w = ImportanceSamplePhase(volMaterial.G[cmp], w); // scattering event...
		}
	}
	return result;
}
