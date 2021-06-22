// Implementation of a simple path tracing in the volume box
float Pathtrace(float3 x, float3 w) {

	float density = GetComponent(Extinction);

	float tMin, tMax;
	if (!BoxIntersect(Grid_Min, Grid_Max, x, w, tMin, tMax))
#ifdef NO_DRAW_LIGHT_SOURCE
		return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#else
		return GetComponent(SampleSkybox(x, w) + SampleLight(w));
#endif

	float d = tMax - tMin;
	x += w * tMin;

	float T = BoxTransmittance(Grid_Min, Grid_Max, density, x, w);

	float3 sky = SampleSkybox(x, w);
#ifdef NO_DRAW_LIGHT_SOURCE
	return T * GetComponent(sky + SampleLight(w));
#else
	return T * GetComponent(sky + SampleLight(w));
#endif
}