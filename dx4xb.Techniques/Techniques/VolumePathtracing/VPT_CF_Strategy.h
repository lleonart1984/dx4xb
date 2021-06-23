// Implements a DT strategy to transmit inside a box

float BoxTransmittance(
	in BOX_INFO box,
	inout float3 x, inout float3 w) {

	float density = GetComponent(Extinction);
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	// Homogenenous hit

	float referenceValue = box.Majorant;

	float t = 0;

	while (true) {
		float d = BoxExit(box.Min_Coord, box.Max_Coord, x, w);
		float h = -log(max(0.000000001, 1 - random())) / max(0.0001, referenceValue);

		if (h >= d) // next scattering event is outside the box
		{
			x = x + w * d;
			return 1;
		}

		x += w * h;
		if (random() < 1 - albedo) // absorption
			return 0;

		complexity++;
		w = ImportanceSamplePhase(g, w); // scattering event...
	}
}