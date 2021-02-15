#include "STBase_RT.h"

//#include "CVAEScatteringModel.h"
#include "CVAEScatteringModelX.h"

float sampleNormal(float mu, float logVar) {
	//return mu + gauss() * exp(logVar * 0.5);
	return mu + gauss() * exp(clamp(logVar, -16, 16) * 0.5);
}

bool GenerateFullVariablesWithModel(float G, float Phi, float3 win, float density, out float3 x, out float3 w, out float3 X, out float3 W, out float factor)
{
	x = float3(0, 0, 0);
	w = win;
	X = x;
	W = w;
	factor = 1;

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
	float lenInput[4];
	float lenOutput[2];
	lenInput[0] = codedDensity;
	lenInput[1] = G;
	lenInput[2] = lenLatent.x;
	lenInput[3] = lenLatent.y;
	lenModel(lenInput, lenOutput);

	float logN = max(0, sampleNormal(lenOutput[0], lenOutput[1]));
	float n = round(exp(logN)+0.49);
	logN = log(n);

	if (random() >= pow(Phi, n))
		return false;

	float4 pathLatent14 = randomStdNormal4();
	float pathLatent5 = randomStdNormal();
	// Generate path
	float pathInput[8];
	float pathOutput[6];
	pathInput[0] = codedDensity;
	pathInput[1] = G;
	pathInput[2] = logN;
	pathInput[3] = pathLatent14.x;
	pathInput[4] = pathLatent14.y;
	pathInput[5] = pathLatent14.z;
	pathInput[6] = pathLatent14.w;
	pathInput[7] = pathLatent5.x;
	pathModel(pathInput, pathOutput);
	float3 sampling = randomStdNormal3();
	float3 pathMu = float3(pathOutput[0], pathOutput[1], pathOutput[2]);
	float3 pathLogVar = float3(pathOutput[3], pathOutput[4], pathOutput[5]);
	float3 pathOut = clamp(pathMu + exp(clamp(pathLogVar, -16, 16) * 0.5) * sampling, -0.9999, 0.9999);
	float costheta = pathOut.x;
	float wt = n >= 2 ? pathOut.y : 0.0; // only if n > 1
	float wb = n >= 3 ? pathOut.z : 0.0; // only if n > 2
	x = float3(0, sqrt(1 - costheta * costheta), costheta);
	float3 N = x;
	float3 B = float3(1, 0, 0);
	float3 T = cross(x, B);

	w = normalize(N * sqrt(max(0, 1 - wt * wt - wb * wb)) + T * wt + B * wb);
	x = mul(x, (R));
	w = mul(w, (R)); // move to radial space

	// Generate Scattering
	float4 scatLatent14 = randomStdNormal4();
	float scatLatent5 = randomStdNormal();
	float scatInput[12];
	float scatOutput[12];
	scatInput[0] = codedDensity;
	scatInput[1] = G;
	scatInput[2] = pow(1 - Phi, 1.0/6.0);
	scatInput[3] = logN;
	scatInput[4] = costheta;
	scatInput[5] = wt;
	scatInput[6] = wb;
	scatInput[7] = scatLatent14.x;
	scatInput[8] = scatLatent14.y;
	scatInput[9] = scatLatent14.z;
	scatInput[10] = scatLatent14.w;
	scatInput[11] = scatLatent5;

	scatModel(scatInput, scatOutput);

	if (n >= 2) {
		X = float3(
			sampleNormal(scatOutput[0], scatOutput[6]),
			sampleNormal(scatOutput[1], scatOutput[7]),
			sampleNormal(scatOutput[2], scatOutput[8]));
		X /= max(1, length(X));
		W = normalize(float3(
			sampleNormal(scatOutput[3], scatOutput[9]),
			sampleNormal(scatOutput[4], scatOutput[10]),
			sampleNormal(scatOutput[5], scatOutput[11])));
		
		X = mul(X, (R));
		W = mul(W, (R)); // move to radial space

		float accum = Phi >= 0.99999 ? n : Phi * (1 - pow(Phi, n)) / (1 - Phi);
		factor = accum / pow(Phi, n);
	}

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

	float3 directContribution = 0;

	bool isOutside = true;

	[loop]
	while (true)
	{
		complexity++;

		RayPayload payload = (RayPayload)0;
		if (!Intersect(x, w, payload)) // 
			return directContribution + importance * (SampleSkybox(w) + SampleLight(w) * (complexity <= 2));

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
			x += t * w; // free traverse in a medium

			bool UsePT = DispatchRaysIndex().x < DispatchRaysDimensions().x* PathtracingRatio;

			if (UsePT)
			{
				if (random() < 1 - volMaterial.ScatteringAlbedo[cmp]) // absorption instead
					return directContribution;

				// Ray cast to light source
				// NEE using straight line as a naive approximation missing refraction at interface
				payload = (RayPayload)0;
				if (!Intersect(x, LightDirection, payload)) // 
					return directContribution; // BAD GEOMETRY!!
				GetHitInfo(
					payload.Barycentric,
					payload.MaterialIndex,
					payload.TriangleIndex,
					payload.VertexOffset,
					payload.TransformIndex,
					surfel, material, volMaterial, 0, 0);

				float d = length(surfel.P - x); // Get medium traversing distance

				payload = (RayPayload)0;
				bool clearPathToLight = !Intersect(surfel.P + LightDirection * 0.01, LightDirection, payload); // Shadow ray

				directContribution += clearPathToLight * importance * LightIntensity * exp(-d * volMaterial.Extinction[cmp]) * EvalPhase(volMaterial.G[cmp], w, LightDirection) / 2;

				w = ImportanceSamplePhase(volMaterial.G[cmp], w); // scattering event...
			}
			else
			{
				float r = MaximalRadius(x, payload.TransformIndex);
				float er = volMaterial.Extinction[cmp] * r;

				float3 _x, _w, _X, _W;
				float factor;
				if (!GenerateFullVariablesWithModel(volMaterial.G[cmp], volMaterial.ScatteringAlbedo[cmp], w, er, _x, _w, _X, _W, factor))
					return directContribution; // case of absorption

				// Gets the sample scattering position for direct lighting
				float3 xs = x + _X * r;

				// Ray cast to light source
				// NEE using straight line as a naive approximation missing refraction at interface
				payload = (RayPayload)0;
				if (!Intersect(xs, LightDirection, payload)) // 
					return directContribution; // BAD GEOMETRY!!
				GetHitInfo(
					payload.Barycentric,
					payload.MaterialIndex,
					payload.TriangleIndex,
					payload.VertexOffset,
					payload.TransformIndex,
					surfel, material, volMaterial, 0, 0);

				float d = length(surfel.P - xs); // Get medium traversing distance

				payload = (RayPayload)0;
				bool clearPathToLight = !Intersect(surfel.P + LightDirection * 0.01, LightDirection, payload); // Shadow ray

				directContribution += factor * clearPathToLight * importance * LightIntensity * exp(-d * volMaterial.Extinction[cmp]) * EvalPhase(volMaterial.G[cmp], _W, LightDirection) / 2;

				w = _w;
				x += _x * r;
			}
		}
	}
}
