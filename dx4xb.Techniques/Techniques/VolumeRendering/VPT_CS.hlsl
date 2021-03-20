#include "VPTBase_CS.h"
#include "..\Tools\HGPhaseFunction.h"


float IntersectSphere(float3 x, float3 w)
{
	//float a = dot(w,w); <- 1 because w is normalized
	float b = 2 * dot(x, w);
	float c = dot(x, x) - 1; // 1 assuming r^2 = 1. r = 1.
	float Disc = b * b - 4 * c;
	// Assuming x is inside the sphere, only the positive root is needed (intersection forward w).
	return (-b + sqrt(Disc)) / 2;
}

/// Function to be replaced by a model!
/// Some performance should arise from avoiding sampling assuming an homogeneous behaviour...
bool TraverseUnitarySphere(out float3 x, inout float3 w, float density, float g, float scatteringAlbedo, inout int counter) {
	x = float3(0, 0, 0); // departing from center
	
	while (true) {
		float d = IntersectSphere(x, w);

		float t = density <= 0.00001 ? 1000000 : -log(max(0.00000000001, 1 - random())) / density;

		x += w * min(t, d);

		if (t >= d) // next scatter occurs outside sphere
			return true;

		if (random() >= scatteringAlbedo)
			return false; // absorption

		counter++;

		w = ImportanceSamplePhase(g, w); // scattering event...
	}
}

#include "MultiscatteringModels.h"

float sampleNormal(float mu, float logVar) {
	//return mu + gauss() * exp(logVar * 0.5);
	return mu + gauss() * exp(clamp(logVar, -16, 16) * 0.5);
}

bool GenerateVariablesWithModel(float G, float Phi, float3 win, float density, out float3 x, out float3 w, inout int counter)
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
	float n = round(exp(logN));
	logN = log(n);

	counter += n;

	if (random() >= pow(Phi, n))
		return false;

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
	float3 pathOut = clamp(pathMu + exp(clamp(pathLogVar, -16, 16) * 0.5) * sampling, -0.9999, 0.9999);
	float costheta = pathOut.x;

	float wt = pathOut.y;
	float wb = n > 1 ? pathOut.z : 0.0; // only if n >= 2 the alpha is different of 0

	//float wt = pathOut.y; 
	//float wb = pathOut.z;

	x = float3(0, sqrt(1 - costheta * costheta), costheta);
	float3 N = x;
	float3 B = float3(1, 0, 0);
	float3 T = cross(x, B);

	//if (logN > log(1)) // multiscattering (FORCING COSINE WEIGHTED DISTRIBUTION)
	//{
	//	float angle = random() * 2 * 3.141596;
	//	float rad = sqrt(random());
	//	wb = rad * sin(angle);
	//	wt = rad * cos(angle);
	//}

	w = normalize(N * sqrt(max(0, 1 - wt * wt - wb * wb)) + T * wt + B * wb);
	x = mul(x, (R));
	w = mul(w, (R)); // move to radial space

	return true;// random() >= 1 - pow(Phi, n);
}


bool EvalTraverseUnitarySphere(out float3 x, inout float3 w, float density, float g, float scatteringAlbedo, inout int counter) {
	float3 win = w;
	if (random() < exp(-density)) // No scattering
	{
		x = w;
		return true;
	}

	return GenerateVariablesWithModel(g, scatteringAlbedo, win, density, x, w, counter);
}

bool TraverseVolume(inout float3 x, float3 w, inout int counter) {

	float density = Extinction[NumberOfPasses % 3];

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	float tMin, tMax;
	if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
		return false;

	x += w * tMin; // move free to box border...

	float d = tMax - tMin;

	while (true) {
		float t = density <= 0.00001 ? 10000000 : -log(max(0.00000000001, 1 - random())) / density;

		x += w * min(t, d);

		if (t >= d)
			return false;

		d -= t;

		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		//counter++;
		float3 probExt = Grid.SampleLevel(VolSampler, tSamplePosition, 0);

		/*float r = 0.01;
		float2 stats = SampleStatistics(x, r, bMin, bMax);
		float probExt = stats.x; // use mean instead*/

		//if (random() < (probExt.z - probExt.y))
		if (random() < probExt.x)
			return true;
	}
}


float3 PathtraceWithoutNEE(float3 x, float3 w, out int counter)
{
	counter = 1;

	float3 importance = 0;
	importance[NumberOfPasses % 3] = 3;

	while (true) {

		if (!TraverseVolume(x, w, counter))
			return importance * (SampleSkybox(w) + SampleLight(w));

		if (random() >= ScatteringAlbedo[NumberOfPasses % 3])
			return 0; // absorption
		w = ImportanceSamplePhase(G[NumberOfPasses % 3], w); // scattering event...
		counter++;
	}
}

float3 PathtraceWithST(float3 x, float3 w, out int counter)
{
	counter = 0;

	float3 importance = 0;
	importance[NumberOfPasses % 3] = 3;

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	int width, height, depth;
	Grid.GetDimensions(width, height, depth);
	float maxDim = max(width, max(height, depth));

	int level = 0;

	while (true) {

		float tMin, tMax;
		if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
			return importance * (SampleSkybox(w) + SampleLight(w));

		x += w * tMin; // move free to box border if outside...

		//  Choose level and ratio
		float3 tSamplePosition = (x - bMin) / (bMax - bMin);
		float3 smp = Grid.SampleLevel(VolSampler, tSamplePosition, level);
		counter++;
		float r = 0.5*(1 << level) / maxDim;

		float3 xs;
		if (!TraverseUnitarySphere(xs, w, r * smp.x * Extinction[NumberOfPasses % 3], G[NumberOfPasses % 3], ScatteringAlbedo[NumberOfPasses % 3], counter))
			return 0; // absorption
		x += xs * r;

		level = (smp.z - smp.y) <= smp.z * 0.001 ? min(6, level + 1) : max(0, level - 1);
	}
}

bool CanBeTreatedAsHomogeneous(float3 gridValue, int numberOfScatters) {
	float max = gridValue.z;
	float min = gridValue.y;
	float ave = gridValue.x;

	//return (max - ave) <= 0.0; // skip only really homogeneous space
	//return false; // no skip
	return (max - ave) <= max * (0.2);
}

float3 PathtraceWithSTSlow(float3 x, float3 w, out int counter)
{
	counter = 0;

	float3 importance = 0; 
	importance[NumberOfPasses % 3] = 3;

	float3 bMin, bMax;
	GetGridBox(bMin, bMax);

	int width, height, depth;
	Grid.GetDimensions(width, height, depth);
	float maxDim = max(width, max(height, depth));
	int numberOfScatters = 1;
	int startLevel = 9;
	while (true) {

		float tMin, tMax;
		if (!BoxIntersect(bMin, bMax, x, w, tMin, tMax))
			return importance * (SampleSkybox(w) + SampleLight(w));

		x += w * tMin; // move free to box border if outside...

		float r, ratio;
		{ //  Choose level and ratio
			float3 tSamplePosition = (x - bMin) / (bMax - bMin);
			int mipLevel = startLevel;
			float3 smp = float3(0, 0, 1);
			while (mipLevel >= 0 && !CanBeTreatedAsHomogeneous(smp, numberOfScatters))
			{
				//counter++;
				smp = Grid.SampleLevel(VolSampler, tSamplePosition, mipLevel);
				mipLevel--;
			}

			int levelInc = 2;// 1 + (smp.z - smp.x <= 0.1 * smp.z);// +(smp.z - smp.y < 0.2);

			startLevel = min(9, mipLevel + levelInc);

			ratio = smp.x;
			r = 0.5*(1 << (mipLevel + 1)) / maxDim;
		}

		//numberOfScatters++;

		float3 xs;
		if (!EvalTraverseUnitarySphere(xs, w, ratio * Extinction[NumberOfPasses % 3] * r, G[NumberOfPasses % 3], ScatteringAlbedo[NumberOfPasses % 3], numberOfScatters))
		//if (!TraverseUnitarySphere(xs, w, ratio * Extinction[NumberOfPasses % 3] * r, G[NumberOfPasses % 3], ScatteringAlbedo[NumberOfPasses % 3], numberOfScatters))
			return 0; // absorption
		counter++;

		x += xs * r;
	}
}


float3 PathtraceWithNEE(float3 x, float3 w, out int counter)
{
	counter = 0;

	float3 importance = 0;
	importance[NumberOfPasses % 3] = 3;

	float3 result = 0;

	while (true) {

		if (!TraverseVolume(x, w, counter))
			return result + importance * SampleSkybox(w);

		//counter++;

		if (random() >= ScatteringAlbedo[NumberOfPasses % 3])
			return result; // absorption

		float3 testX = x;
		result += (!TraverseVolume(testX, LightDirection, counter)) * importance * LightIntensity * EvalPhase(G[NumberOfPasses % 3], w, LightDirection);

		w = ImportanceSamplePhase(G[NumberOfPasses % 3], w); // scattering event...
	}
}

float3 Pathtrace(float3 x, float3 w, out int counter)
{
	//return PathtraceWithNEE(x, w, counter);
	//return PathtraceWithoutNEE(x, w, counter);
	return PathtraceWithSTSlow(x, w, counter);
}