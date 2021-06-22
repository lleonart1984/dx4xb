/*
bool BoxTransmittance(
	float3 bMin, float3 bMax,
	float majorant,
	inout float3 x, inout float3 w);
	*/

// Implementation of a simple path tracing in the volume box
float Pathtrace(float3 x, float3 w) {
	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float density = GetComponent(Extinction);

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
#ifdef NO_DRAW_LIGHT_SOURCE
		return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#else
		return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#endif

	float d = tMax - tMin;
	x += w * tMin;
	
	if (!BoxTransmittance(bMin, bMax, density, x, w))
		return 0; // Absorption

	float3 sky = SampleSkybox(x, w);
#ifdef NO_DRAW_LIGHT_SOURCE
	return GetComponent(sky + SampleLight(w));
#else
	return GetComponent(sky + SampleLight(w));
#endif
}