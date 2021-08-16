#include "VST_Hom_Base.h"

#include "MultiscatteringModels.h"

float sampleNormal(float mu, float logVar) {
	//return mu + gauss() * exp(logVar * 0.5);
	return mu + gauss() * exp(clamp(logVar, -16, 16) * 0.5);
}

float GenerateVariablesWithModel(float G, float Phi, float3 win, float density, out float3 x, out float3 w)
{
	x = float3(0, 0, 0);
	w = win;

	float3 temp = abs(win.x) >= 0.9999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 winY = normalize(cross(temp, win));
	float3 winX = cross(win, winY);
	float rAlpha = random() * 2 * pi;
	float3x3 R = (mul(float3x3(
		cos(rAlpha), -sin(rAlpha), 0,
		sin(rAlpha), cos(rAlpha), 0,
		0, 0, 1), float3x3(winX, winY, win)));

	float codedDensity = density;// pow(density / 400.0, 0.125);

	float2 lenLatent = randomStdNormal2();
	// Generate length
	float lenInput[3];
	float lenOutput[2];
	lenInput[0] = codedDensity;
	lenInput[1] = G;
	lenInput[2] = lenLatent.x;
	//lenInput[3] = lenLatent.y;
	lenModel(lenInput, lenOutput);

	float logN = max(0, sampleNormal(lenOutput[0], lenOutput[1]));
	float n = (exp(logN));// round(exp(logN) + 0.49);
	//logN = log(n);

	if (random() >= pow(Phi, n))
		return 0;

	scatters += n;

	float4 pathLatent14 = randomStdNormal4();
	//float pathLatent5 = randomStdNormal();
	// Generate path
	float pathInput[6];
	float pathOutput[6];
	pathInput[0] = codedDensity;
	pathInput[1] = G;
	pathInput[2] = logN;
	pathInput[3] = pathLatent14.x;
	pathInput[4] = pathLatent14.y;
	pathInput[5] = pathLatent14.z;
	//pathInput[6] = pathLatent14.w;
	//pathInput[7] = pathLatent5.x;
	pathModel(pathInput, pathOutput);
	float3 sampling = randomStdNormal3();
	float3 pathMu = float3(pathOutput[0], pathOutput[1], pathOutput[2]);
	float3 pathLogVar = float3(pathOutput[3], pathOutput[4], pathOutput[5]);
	float3 pathOut = clamp(pathMu + exp(clamp(pathLogVar, -16, 16) * 0.5) * sampling, -0.99999, 0.99999);
	float costheta = pathOut.x;

	float wt = pathOut.y;
	float wb = pathOut.z;// n >= 2 ? pathOut.z : 0.0; // only if n >= 2 the alpha is different of 0

	x = float3(0, sqrt(1 - costheta * costheta), costheta);
	float3 N = x;
	float3 B = float3(1, 0, 0);
	float3 T = cross(x, B);

	w = normalize(N * sqrt(max(0, 1 - wt * wt - wb * wb)) + T * wt + B * wb);
	x = mul(x, (R));
	w = mul(w, (R)); // move to radial space

	return 1;// random() >= 1 - pow(Phi, n);
}

float IntersectSphere(float3 x, float3 w)
{
	float xDotW = dot(x, w);
	float xDotX = dot(x, x);
	return sqrt(xDotW * xDotW - xDotX + 1) - xDotW;
}


float UnitarySphereHomogeneousTransmittance(float density, float g, float albedo, inout float3 w, out float3 x)
{
	if (density < 5)
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

	if (random() < exp(-density)) // No scattering
	{
		x = w;
		return 1;
	}

	float3 win = w;
	return GenerateVariablesWithModel(g, albedo, win, density, x, w);
}
