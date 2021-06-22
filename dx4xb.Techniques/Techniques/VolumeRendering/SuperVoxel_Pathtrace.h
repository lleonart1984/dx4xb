Texture3D<float> Majorants : register(t1);






// Implementation of a simple path tracing in the volume box
float Pathtrace(float3 x, float3 w) {
	float density = GetComponent(Extinction); // extinction coefficient multiplier.
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	int3 dimensions;
	Grid.GetDimensions(dimensions.x, dimensions.y, dimensions.z);
	
	float3 lastScatter = x;


	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax) || density <= 0.00001)
#ifdef NO_DRAW_LIGHT_SOURCE
		return GetComponent(SampleSkybox(lastScatter, w));
#else
		return GetComponent(SampleSkybox(lastScatter, w) + SampleLight(w));
#endif

	float d = tMax - tMin;
	x += w * tMin;

	int3 cell = (x - bMin) * dimensions / (bMax - bMin);
	cell = clamp(cell, 0, dimensions - 1);

	int scatters = 0;

	while (true) {

		float3 xs;
		float T = Transmittance(x, w, d, density, xs);

		if (random() < T) // no scattering event
		{
			float3 sky = SampleSkybox(lastScatter, w);
#ifdef NO_DRAW_LIGHT_SOURCE
			return GetComponent(sky + SampleLight(w) * (scatters > 0));
#else
			return GetComponent(sky + SampleLight(w));
#endif
		}

		x = xs; // move to next event
		lastScatter = x;
		scatters++;

		if (random() >= albedo) // absorption
			return 0; // Le(x) if emissive...

		w = ImportanceSamplePhase(g, w); // scattering event...

		BoxIntersect(bMin, bMax, x, w, tMin, tMax);
		//	return GetComponent(float3(1, 0, 1));// SampleSkybox(lastScatter, w)); // only possible if numerical errors....

		d = tMax - tMin;
		x += w * tMin;
	}
}