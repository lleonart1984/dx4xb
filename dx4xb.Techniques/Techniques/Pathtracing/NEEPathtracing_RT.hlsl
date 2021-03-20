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

	float3 directContribution = 0;

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
			return directContribution + importance * SampleSkybox(w);

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

		directContribution += importance * material.Emissive;

		//return normalize(surfel.N);
		//return dot(-D, surfel.N) > 0 ? float3(1, 1, 1) : float3(1, 0, 0);

		float d = length(surfel.P - x); // Distance to the hit position.
		float t = isOutside || volMaterial.Extinction[cmp] == 0 ? 100000000 : -log(max(0.000000000001, 1 - random())) / volMaterial.Extinction[cmp];

		[branch]
		if (t >= d)
		{
			bounces += isOutside;
			if (bounces >= MAX_PATHTRACING_BOUNCES)
				return directContribution;

			payload = (RayPayload)0;
			bool clearPathToLight = !Intersect(surfel.P + surfel.N * 0.0001, LightDirection, payload); // Shadow ray
			directContribution += clearPathToLight * importance * LightIntensity * DirectContribution(-w, LightDirection, surfel.N, dot(surfel.N, LightDirection), material);

			SurfelScattering(x, w, importance, surfel, material);

			if (any(material.Specular) && material.Roulette.w > 0)
				isOutside = dot(surfel.N, w) >= 0;
		}
		else
		{ // Volume scattering or absorption
			x += t * w; // free traverse in a medium
			if (random() < 1 - volMaterial.ScatteringAlbedo[cmp]) // absorption instead
				return directContribution;

			// Ray cast to light source
			// NEE using straight line as a naive approximation missing refraction at interface
			payload = (RayPayload)0;
			if (!Intersect(x, LightDirection, payload)) // 
				return directContribution; // BAD GEOMETRY!!
			GetHitInfo(
				payload.Barycentric,
				payload.MaterialIndex,
				payload.TriangleIndex,
				payload.VertexOffset,
				payload.TransformIndex,
				surfel, material, volMaterial, 0, 0);

			float d = length(surfel.P - x); // Get medium traversing distance

			payload = (RayPayload)0;
			bool clearPathToLight = !Intersect(surfel.P + LightDirection * 0.001, LightDirection, payload); // Shadow ray

			directContribution += clearPathToLight * importance * LightIntensity * exp(-d * volMaterial.Extinction[cmp]) * EvalPhase(volMaterial.G[cmp], w, LightDirection);

			w = ImportanceSamplePhase(volMaterial.G[cmp], w); // scattering event...
		}
	}
	return 0;
}
