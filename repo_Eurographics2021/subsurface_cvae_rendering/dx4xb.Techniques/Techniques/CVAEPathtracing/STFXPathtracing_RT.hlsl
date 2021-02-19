// TABULAR METHOD (EXTENDED TO PRODUCE OUTGOING DIRECTION)
// The pdf table is generated offline and passed here as a buffer.
// Constansts below define the bins similar to the distribution at:
// Efficient Rendering of Heterogeneous Polydisperse Granular Media
// of Mueller et at.

#include "STBase_RT.h"

// HG factor [-1,1] linear
#define BINS_G 100

// Log of the number of Scatters between 0..8
#define BINS_LOGN 100

// Radius [0, 127] linear in log(r)
#define BINS_R 8

// Theta [0,pi] linear in cos(theta) beta[-1,1] alpha[-1,1]
//40x20x10
#define BINS_THETA 40
#define BINS_BETA 20
#define BINS_ALPHA 10
#define BINS_X (BINS_THETA * BINS_BETA * BINS_ALPHA)

#define LOGN_G_STRIDE (BINS_R * BINS_LOGN)
#define LOGN_R_STRIDE BINS_LOGN

#define XW_G_STRIDE (BINS_R * BINS_LOGN * BINS_X)
#define XW_R_STRIDE (BINS_LOGN * BINS_X)
#define XW_LOGN_STRIDE (BINS_X)

// PDF of the number of scatters
// The table is storing actually the empirical cdf(logN | g, r)
StructuredBuffer<float> CDF_LogN					: register(t0, space2);
// PDF of the outgoing position
// The table is storing actually the empirical cdf(x, w | g, logN, r)
// The table is split in two tables to allow more than 2G memory table
StructuredBuffer<float> CDF_XW_L					: register(t1, space2);
StructuredBuffer<float> CDF_XW_H					: register(t2, space2);

int SearchBin(StructuredBuffer<float> cdf, int beg, int end, float value) {
	while (beg < end) {
		int med = (beg + end) / 2;
		if (value < cdf[med])
			end = med;
		else
			beg = med + 1;
	}
	return beg;
}

// Samples an outgoing position and direction using the tables
// Return true if the sample exit, false if ray is absorbed.
bool SampleCosXAndW(float g, float phi, float r, out float theta, out float beta, out float alpha)
{
	int rBin = 0;
	if (r < 1) {
		rBin = random() < r; // 0 or 1 depending on linerly on r (0,1].
	}
	else
	{
		float logR = log2(r) - 0.5 + 1;
		rBin = logR;
		rBin += random() < (logR % 1);
	}
	int gBin = min((g * 0.5 + 0.5) * BINS_G, BINS_G - 1);

	// Get the logN from the table
	int startPoslogNPos = gBin * LOGN_G_STRIDE + rBin * LOGN_R_STRIDE;
	int selectedLogNBin = SearchBin(CDF_LogN, startPoslogNPos, startPoslogNPos + BINS_LOGN - 1, random()) - startPoslogNPos;
	float logN = 8.0 * (selectedLogNBin + random()) / BINS_LOGN;

	int N = (exp(logN));

	if (random() >= pow(phi, N)) // Absorption given N
		return false;

	/*theta = random() * 2 - 1;

	float angle = random() * 2 * 3.141596;
	float rad = sqrt(random());
	alpha = rad * sin(angle);
	beta = rad * cos(angle);

	return true;*/

	bool sampleLow = true;
	if (startPoslogNPos >= BINS_G * BINS_R * BINS_LOGN / 2) // Sampling from high density pdfs
	{
		sampleLow = false;
		startPoslogNPos -= BINS_G * BINS_R * BINS_LOGN / 2; // Move the ptr respect to the second buffer.
	}

	int startxwPos = (startPoslogNPos + selectedLogNBin) * BINS_X;

	int xwBin;
	if (sampleLow)
		xwBin = SearchBin(CDF_XW_L, startxwPos, startxwPos + BINS_X - 1, random()) - startxwPos;
	else
		xwBin = SearchBin(CDF_XW_H, startxwPos, startxwPos + BINS_X - 1, random()) - startxwPos;

	// Convert xwBin into theta, beta, alpha bins...
	int thetaBin = xwBin / (BINS_BETA * BINS_ALPHA);
	int betaBin = (xwBin % (BINS_BETA * BINS_ALPHA)) / BINS_ALPHA;
	int alphaBin = xwBin % BINS_ALPHA;

	theta = (thetaBin + random()) * 2.0 / BINS_THETA - 1.0;
	beta = N > 1 ? (betaBin + random()) * 2.0 / BINS_BETA - 1.0 : 0.0;
	alpha = N > 2 ? (alphaBin + random()) * 2.0 / BINS_ALPHA - 1.0 : 0.0;
	return true;
}

bool GenerateVariablesWithTable(float G, float Phi, float3 win, float density, out float3 x, out float3 w)
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

	float theta, beta, alpha;
	if (!SampleCosXAndW(G, Phi, density, theta, beta, alpha))
		return false;

	x = float3(0, sqrt(1 - theta * theta), theta);
	float3 N = x;
	float3 B = float3(1, 0, 0);
	float3 T = cross(x, B);
	w = normalize(N * sqrt(max(0, 1 - beta * beta - alpha * alpha)) + T * beta + B * alpha);

	x = mul(x, R);
	w = mul(w, R); // move to radial space

	return true;
}

float3 ComputePath(float3 O, float3 D, inout int complexity)
{
	int cmp = NumberOfPasses % 3;
	float3 importance = 0;
	importance[cmp] = 3;
	float3 x = O;
	float3 w = D;

	bool inMedium = false;

	int bounces = 0;

	float3 result = 0;

	bool isOutside = true;

	[loop]
	while (true)
	{
		complexity++;

		RayPayload payload = (RayPayload)0;
		if (!Intersect(x, w, payload)) // 
			return importance * (SampleSkybox(w) + SampleLight(w) * (bounces > 0));
		
		Vertex surfel = (Vertex)0;
		Material material = (Material)0;
		VolumeMaterial volMaterial = (VolumeMaterial)0;
		GetHitInfo(
			payload.Barycentric,
			payload.MaterialIndex,
			payload.TriangleIndex,
			payload.VertexOffset,
			payload.TransformIndex,
			surfel, material, volMaterial, 0, 0);

		float d = length(surfel.P - x); // Distance to the hit position.
		float t = isOutside || volMaterial.Extinction[cmp] == 0 ? 100000000 : -log(max(0.000000000001, 1 - random())) / volMaterial.Extinction[cmp];

		[branch]
		if (t >= d)
		{
			bounces += isOutside;

			if (bounces >= MAX_PATHTRACING_BOUNCES)
				return 0;

			SurfelScattering(x, w, importance, surfel, material);

			if (any(material.Specular) && material.Roulette.w > 0)
				isOutside = dot(surfel.N, w) >= 0;
		}
		else
		{ // Volume scattering or absorption
			bool UsePT = DispatchRaysIndex().x < DispatchRaysDimensions().x* PathtracingRatio;

			if (UsePT) {
				x += t * w; // free traverse in a medium

				if (random() < 1 - volMaterial.ScatteringAlbedo[cmp]) // absorption instead
					return 0;

				w = ImportanceSamplePhase(volMaterial.G[cmp], w); // scattering event...
			}
			else
			{
				float r = MaximalRadius(x, payload.TransformIndex);

				float er = volMaterial.Extinction[cmp] * r;

				if (er >= 1) {
					er = min(er, 64);

					float3 _x, _w, _X, _W;

					if (!GenerateVariablesWithTable(volMaterial.G[cmp], volMaterial.ScatteringAlbedo[cmp], w, er, _x, _w))
						return 0;

					w = _w;
					x += _x * er / volMaterial.Extinction[cmp];
				}
				else {
					x += t * w; // free traverse in a medium

					if (random() < 1 - volMaterial.ScatteringAlbedo[cmp]) // absorption instead
						return 0;

					w = ImportanceSamplePhase(volMaterial.G[cmp], w); // scattering event...

				}
			}
		}
	}
}
