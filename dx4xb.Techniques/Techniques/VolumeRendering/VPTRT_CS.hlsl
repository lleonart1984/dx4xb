#include "VPTBase_CS.h"
#include "..\Tools\HGPhaseFunction.h"

sampler VolPointSampler : register(s0, space1);

// Implementation of a delta-tracking

float3 DTPathtrace(float3 x, float3 w, out int counter) {
	counter = 0;

	float density = Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float3 W = 0; // weight
	W[NumberOfPasses % 3] = 3; // Wavelenght dependent importance multiplier

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return W * (SampleSkybox(w) + SampleLight(w));

	float d = tMax - tMin;
	x += w * tMin;

	while (true) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / density; // majorant of all volume data

		if (t >= d)
			return W * (SampleSkybox(w) + SampleLight(w));

		x += w * t; // move to next event position or border
		
		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float probExt = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x;
		counter++; // grid lookups
		
		float m_t = probExt * density; // extinction coef
		float m_s = m_t * ScatteringAlbedo[NumberOfPasses % 3]; // scattering coef
		float m_a = m_t - m_s; // absorption coef
		float m_n = density - m_t; // null coef

		float xi = random();

		float Pa = m_a / density;
		float Ps = m_s / density;
		float Pn = m_n / density;

		if (xi < Pa) // absorption
			return 0;

		if (xi < 1 - Pn) // scattering
		{
			w = ImportanceSamplePhase(G[NumberOfPasses % 3], w); // scattering event...

			if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
				return W * (SampleSkybox(w) + SampleLight(w));
			
			d = tMax - tMin;
			x += w * tMin;
		}
		else {
			// if no absorption and no scattering null collision occurred
			d -= t;
		}
	}
}

float3 SVPathtrace(float3 x, float3 w, out int counter) {

}

void RatioTrack(float3 x, float3 w, out float T, out float3 xs)
{
	float density = Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.
	float majorant = density;

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	xs = x;
	T = 1;
	float A = 0.000001;

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return;

	float d = tMax - tMin;
	x += w * tMin;

	while (T > 0.0001) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / majorant; // majorant of all volume data

		if (t >= d)
			return;

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float mu = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x * density;

		float Ps = T * mu / majorant;
		A += Ps;
		T *= (1 - mu / majorant);

		if (random() < Ps / A)
			xs = x;

		d -= t;
	}
}

void ResidualRatioTrack(float3 x, float3 w, out float T, out float3 xs) {
	float density = Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	xs = x;
	T = 1;
	float A = 0.000001;

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return;

	float d = tMax - tMin;
	x += w * tMin;

	float mu_c = 0.2 * density;
	float mu_r = max(1 - mu_c, mu_c) * density;

	float Tc = exp(-mu_c * d);
	float Tr = 1;

	while (Tr > 0.0001) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / mu_c; // majorant of all volume data

		if (t >= d) {
			T = Tc * Tr;
			return;
		}

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float mu = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x * density;

		float Ps = (Tc * Tr) * mu;
		A += Ps;

		if (random() < Ps / A)
			xs = x;

		Tr *= (1 - (mu - mu_c) / mu_r);

		d -= t;
	}
}

// Ratio-tracking implementation
float3 RTPathtrace(float3 x, float3 w, out int counter) {
	counter = 0;

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float3 W = 0; // weight
	W[NumberOfPasses % 3] = 3; // Wavelenght dependent importance multiplier

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return W * (SampleSkybox(w) + SampleLight(w));

	x += w * tMin;

	float3 accum = 0;

	while (true) {
		float T;
		float3 xs;
		RatioTrack(x, w, T, xs);

		accum += T * W * (SampleSkybox(w) + SampleLight(w));

		if (random() < T) // no scatter event
			break;

		w = ImportanceSamplePhase(G[NumberOfPasses % 3], w); // scattering event...
		x = xs;
	}

	return accum;
}

// DT Transmittance
float3 DTTransmittance(float3 x, float3 w) {
	float density = 10;// Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float3 W = 0; // weight
	W[NumberOfPasses % 3] = 3; // Wavelenght dependent importance multiplier

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return 0;// W* (SampleSkybox(w) + SampleLight(w));

	float d = tMax - tMin;
	x += w * tMin;

	while (true) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / density; // majorant of all volume data

		if (t >= d)
			return 0;

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float probExt = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x;

		if (random() < probExt) // scattering
		{
			return 1;
		}
		else {
			// if no absorption and no scattering null collision occurred
			d -= t;
		}
	}
}

// Ratio-Tracking Transmittance
float3 RTTransmittance(float3 x, float3 w) {
	float density = 10;// Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float3 W = 0; // weight
	W[NumberOfPasses % 3] = 3; // Wavelenght dependent importance multiplier

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return 0;// W* (SampleSkybox(w) + SampleLight(w));

	float d = tMax - tMin;
	x += w * tMin;

	float T = 1;

	while (true) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / density; // majorant of all volume data

		if (t >= d)
			return 1 - T;

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float probExt = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x;

		T *= (1 - probExt);
		d -= t;
	}
}

// Residual-Ratio-Tracking Transmittance
float3 RRTTransmittance(float3 x, float3 w) {
	float density = 10;// Extinction[NumberOfPasses % 3]; // extinction coefficient multiplier.

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float3 W = 0; // weight
	W[NumberOfPasses % 3] = 3; // Wavelenght dependent importance multiplier

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return 0;// W* (SampleSkybox(w) + SampleLight(w));

	float d = tMax - tMin;
	x += w * tMin;

	float mu_c = 0.2 * density;
	float mu_r = max(1 - mu_c, mu_c) * density;

	float T = 1;
	float Tc = exp(-mu_c * d);

	while (true) {

		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / mu_r; // majorant of all volume data

		if (t >= d)
			return 1 - Tc * T;

		x += w * t; // move to next event position or border

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float mu = Grid.SampleLevel(VolSampler, tSamplePosition, 0).x * density;

		T *= (1 - (mu - mu_c) / mu_r);
		d -= t;
	}
}

float3 Pathtrace(float3 x, float3 w, out int counter)
{
	return DTPathtrace(x, w, counter);
	//return RTPathtrace(x, w, counter);

	//counter = 0;
	//return RRTTransmittance(x, w);
	//return DTTransmittance(x, w);
}