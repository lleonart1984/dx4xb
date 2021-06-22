// Residual-Ratio-Tracking Transmittance
float3 Transmittance(float3 x, float3 w, float d, float majorant, out float3 xs) {
	
	if (majorant <= 0.0001)
	{
		xs = x + w * d;
		return 1;
	}

	float density = GetComponent(Extinction); // extinction coefficient multiplier.

	float t = 0;
	
	float mu_c = 0.2 * majorant;
	float mu_r = max(1 - mu_c, mu_c) * majorant;

	float T = 1;
	float Tc = exp(-mu_c * d);

	float total = 0;

	float3 xt = x;

	while (true) {

		float t -= log(max(0.00000000001, 1 - random())) / mu_r; // majorant of all volume data

		xt = x + w * min(t,d); // move to next eveent position or border

		if (t >= d)
			return 1 - Tc * T;


		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float mu = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x * density;

		T *= (1 - (mu - mu_c) / mu_r);
		d -= t;
	}
}