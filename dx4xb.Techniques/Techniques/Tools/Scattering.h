#ifndef SCATTERING_H
#define SCATTERING_H

#include "Randoms.h"
#include "Definitions.h"

// Compute fresnel reflection component given the cosine of input direction and refraction index ratio.
// Refraction can be obtained subtracting to one.
// Uses the Schlick's approximation
float ComputeFresnel(float NdotL, float ratio)
{
	float oneMinusRatio = 1 - ratio;
	float onePlusRatio = 1 + ratio;
	float divOneMinusByOnePlus = oneMinusRatio / onePlusRatio;
	float f = divOneMinusByOnePlus * divOneMinusByOnePlus;
	return (f + (1.0 - f) * pow((1.0 - NdotL), 5));
}

// Gets perfect lambertian normalized brdf ratio divided by a uniform distribution pdf value
float3 DiffuseBRDFMulTwoPi(float3 V, float3 L, float3 fN, float NdotL, Material material)
{
	return material.Diffuse * 2;
}

// Gets perfect lambertian normalized brdf ratio
float3 DiffuseBRDF(float3 V, float3 L, float3 fN, float NdotL, Material material) {
	return material.Diffuse / pi;
}

// Gets blinn-model specular normalized brdf ratio divided by a uniform distribution pdf value
float3 SpecularBRDFMulTwoPi(float3 V, float3 L, float3 fN, float NdotL, Material material)
{
	float3 H = normalize(V + L);
	float HdotN = max(0.0001, dot(H, fN));
	return pow(HdotN, material.SpecularSharpness) * material.Specular * (2 + material.SpecularSharpness);
}

// Gets blinn-model specular normalized brdf ratio
float3 SpecularBRDF(float3 V, float3 L, float3 fN, float NdotL, Material material)
{
	float3 H = normalize(V + L);
	float HdotN = max(0.0001, dot(H, fN));
	return pow(HdotN, material.SpecularSharpness) * material.Specular * (2 + material.SpecularSharpness) / two_pi;
}

// Scatters a ray randomly using the material roulette information
// to decide between different illumination models (lambert, blinn, mirror, fresnel).
// the inverse of pdf value is already multiplied in ratio
// This method overload can use precomputed Reflectance (R) and Transmittance (T) vectors
void RandomScatterRay(float3 V, float3 fN, float4 R, float4 T, Material material,
	out float3 ratio,
	out float3 direction,
	out float pdf
) {
	float NdotD;
	float3 D = randomHSDirection(fN, NdotD);

	float3 Diff = DiffuseBRDFMulTwoPi(V, D, fN, NdotD, material);
	float3 Spec = SpecularBRDFMulTwoPi(V, D, fN, NdotD, material);

	float4x3 ratios = {
		Diff * NdotD, // Diffuse
		Spec * NdotD, // Specular (Glossy)
		material.Specular, // Mirror
		material.Specular  // Fresnel
	};

	float4x3 directions = {
		D,
		D, // Could be improved using some glossy distribution
		R.xyz,
		T.xyz
	};

	float4 roulette = float4(material.Roulette.x, material.Roulette.y, R.w, T.w);

	float4 lowerBound = float4(0, roulette.x, roulette.x + roulette.y, roulette.x + roulette.y + roulette.z);
	float4 upperBound = lowerBound + roulette;

	float4 scatteringSelection = random();

	float4 selectionMask =
		(lowerBound <= scatteringSelection)
		* (scatteringSelection < upperBound);

	ratio = mul(selectionMask, ratios);
	direction = mul(selectionMask, directions);
	pdf = dot(selectionMask, roulette);
}

// Scatters a ray randomly using the material roulette information
// to decide between different illumination models (lambert, blinn, mirror, fresnel).
// the inverse of pdf value is already multiplied in ratio
void RandomScatterRay(float3 V, Vertex surfel, Material material,
	out float3 ratio,
	out float3 direction,
	out float pdf
) {
	// compute cosine of angle to viewer respect to the surfel Normal
	float NdotV = dot(V, surfel.N);
	// detect if normal is inverted
	bool invertNormal = NdotV < 0;
	NdotV = abs(NdotV); // absolute value of the cosine
	// Ratio between refraction indices depending of exiting or entering to the medium (assuming vaccum medium 1)
	float eta = invertNormal ? material.RefractionIndex : 1 / material.RefractionIndex;
	// Compute faced normal
	float3 fN = invertNormal ? -surfel.N : surfel.N;
	// Gets the reflection component of fresnel
	float4 R = float4(reflect(-V, fN), ComputeFresnel(NdotV, eta));
	// Compute the refraction component
	float4 T = float4(refract(-V, fN, eta), 1 - R.w);

	if (!any(T.xyz))
	{
		R.w = 1;
		T.w = 0; // total internal reflection
	}
	// Mix reflection impulse with mirror materials
	R.w = material.Roulette.z + R.w * material.Roulette.w;
	T.w *= material.Roulette.w;

	RandomScatterRay(V, fN, R, T, material, ratio, direction, pdf);
}

void ComputeImpulses(float3 V, Vertex surfel, Material material,
	out float NdotV,
	out bool invertNormal,
	out float3 fN,
	out float4 R,
	out float4 T) {
	// compute cosine of angle to viewer respect to the surfel Normal
	NdotV = dot(V, surfel.N);
	// detect if normal is inverted
	invertNormal = NdotV < 0;
	NdotV = abs(NdotV); // absolute value of the cosine
	// Ratio between refraction indices depending of exiting or entering to the medium (assuming vaccum medium 1)
	float eta = !invertNormal ? material.RefractionIndex : 1 / material.RefractionIndex;
	// Compute faced normal
	fN = invertNormal ? -surfel.N : surfel.N;
	// Gets the reflection component of fresnel
	R = float4(reflect(-V, fN), ComputeFresnel(NdotV, eta));
	// Compute the refraction component
	T = float4(refract(-V, fN, eta), 1 - R.w);

	if (!any(T.xyz))
	{
		R.w = 1;
		T.w = 0; // total internal reflection
	}
	// Mix reflection impulse with mirror materials
	R.w = material.Roulette.z + R.w * material.Roulette.w;
	T.w *= material.Roulette.w;
}
#endif // !SCATTERING_TOOLS_H

