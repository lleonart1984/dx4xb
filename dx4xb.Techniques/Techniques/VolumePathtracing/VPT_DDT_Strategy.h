// Implements a Decomposition DT strategy to transmit inside a box
float BoxTransmittance(
	in BOX_INFO box,
	inout float3 x, inout float3 w) {

	float density = GetComponent(Extinction);
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	float d = BoxExit(box.Min_Coord, box.Max_Coord, x, w);

	// Homogenenous hit
	float h = -log(max(0.000000001, 1 - random())) / max(0.0001, box.Minorant);
	
	float t = 0;

	while (true) {

		t -= log(max(0.000000001, 1 - random())) / max(0.0001, box.Majorant - box.Minorant);

		if (t >= d && h >= d) // next scattering event is outside the box
		{
			x = x + w * d;
			return 1;
		}

		bool scatter = false;
		float3 xs = x + w * min(h, t); // next-scattering in heterogeneous residual
		if (h <= t) {
			scatter = true;
		}
		else
			if (random() * (box.Majorant - box.Minorant) < (SampleGrid(xs) * density - box.Minorant))
				scatter = true;

		if (scatter) // scatter event
		{
			x = xs;
			if (random() < 1 - albedo) // absorption
				return 0;
			w = ImportanceSamplePhase(g, w); // scattering event...
			d = BoxExit(box.Min_Coord, box.Max_Coord, x, w);
			t = 0;
			h = -log(max(0.000000001, 1 - random())) / max(0.0001, box.Minorant);
		}
	}
}