// DT Transmittance
float Transmittance(
	float3 x, float3 w, float d, float majorant,
	out float3 xs) {

	if (majorant <= 0.0001)
	{
		xs = x + w * d;
		return 1;
	}

	float density = GetComponent(Extinction); // extinction coefficient multiplier.

	float t = 0;

	while (true) {

		t -= log(max(0.0000000001, 1 - random())) / majorant; // majorant of the path x, x + w * d

		xs = x + w * min(t, d);

		if (t >= d)
			return 1;

		float sigma_x = SampleGrid(xs) * density;

		if (random() * majorant < sigma_x) // scattering
			return 0;
		// else Null collision... continue...
	}
}