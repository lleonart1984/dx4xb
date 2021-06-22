// Implements a DT strategy to transmit inside a box

float BoxTransmittance(
	float3 bMin, float3 bMax,
	float majorant,
	inout float3 x, inout float3 w) {

	float density = GetComponent(Extinction);
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	float tMin, tMax;
	BoxIntersect(bMin, bMax, x, w, tMin, tMax);

	float d = tMax - tMin;//BoxExit(bMin, bMax, x, w);
	x += w * tMin;
	float t = 0;

	while (true) {

		t -= log(max(0.000000001, 1 - random())) / max(0.0001, majorant);

		if (t >= d) // exit box
		{
			x = x + w * d;
			return 1;
		}

		float3 xs = x + w * t;

		if (random() < SampleGrid(xs)*density/majorant) // scatter event
		{
			x = xs;
			
			if (random() < 1 - albedo) // absorption
				return 0;
			
			w = ImportanceSamplePhase(g, w); // scattering event...

			BoxIntersect(bMin, bMax, x, w, tMin, tMax);
			d = tMax - tMin;
			x += w * tMin;
			//d = BoxExit(bMin, bMax, x, w);

			t = 0;
		}
	}
}