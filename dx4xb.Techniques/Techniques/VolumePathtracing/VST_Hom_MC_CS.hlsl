#include "VST_Hom_Base.h"

float IntersectSphere2(float3 x, float3 w)
{
	//float a = dot(w,w); <- 1 because w is normalized
	float b = 2 * dot(x, w);
	float c = dot(x, x) - 1; // 1 assuming r^2 = 1. r = 1.
	float Disc = b * b - 4 * c;
	// Assuming x is inside the sphere, only the positive root is needed (intersection forward w).
	return (-b + sqrt(Disc)) / 2;
}

float IntersectSphere(float3 x, float3 w)
{
	float xDotW = dot(x, w);
	float xDotX = dot(x, x);
	return sqrt(xDotW * xDotW - xDotX + 1) - xDotW;
}


float UnitarySphereHomogeneousTransmittance(float density, float g, float albedo, inout float3 w, out float3 x)
{
	x = float3(0, 0, 0); // departing from center

	while (true) {
		float d = IntersectSphere(x, w);

		float t = density <= 0.00001 ? 1000000 : -log(max(0.00000000001, 1 - random())) / density;

		x += w * min(t, d);

		if (t >= d) // next scatter occurs outside sphere
			return 1;

		if (random() >= albedo)
			return 0; // absorption

		w = ImportanceSamplePhase(g, w); // scattering event...
		scatters++;
	}
}
