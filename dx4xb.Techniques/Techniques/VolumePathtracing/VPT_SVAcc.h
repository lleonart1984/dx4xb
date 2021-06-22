Texture3D<float> Majorants : register(t1);

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

	x += w * tMin;

	int3 dimensions;
	Grid.GetDimensions(dimensions.x, dimensions.y, dimensions.z);

	int3 mdimensions = int3(ceil(dimensions / (float)SV_SIZE)); // majorant dimensions

	int3 cell = int3((x - bMin) * dimensions / (bMax - bMin)) / SV_SIZE;
	cell = clamp(cell, 0, mdimensions - 1);

	float3 cellSize = SV_SIZE * (bMax - bMin) / dimensions;

	float T = 1;

	int b = 0;

	while (true) {
		//complexity++;
		
		float3 cellMin = bMin + cell * cellSize;
		float3 cellMax = cellMin + cellSize;

		float3 xc = (cellMax + cellMin) * 0.5;

		complexity++;
		T *= BoxTransmittance(cellMin, cellMax, Majorants[cell]*density, x, w);

		if (T <= 0.00001)
			return 0; // stop if absorption exceds a threshold

		float3 mov = abs(x + w*0.001 - xc);

		int3 cellmov = int3(mov.x >= mov.y && mov.x >= mov.z, mov.y >= mov.x && mov.y >= mov.z, mov.z >= mov.x && mov.z >= mov.y) * sign (x - xc);
		
		cell += cellmov;

		if (any(cell < 0) || any(cell >= mdimensions)) // go to a cell outside volume
		{
			float3 sky = SampleSkybox(x, w);
#ifdef NO_DRAW_LIGHT_SOURCE
			return T * GetComponent(sky + SampleLight(w));
#else
			return T * GetComponent(sky + SampleLight(w));
#endif
		}
	}

	return 0;
}