#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Material definition info
struct Material
{
	float3 Diffuse; // Lambertian model
	float RefractionIndex;
	float3 Specular; // Blinn-Phong glossy model
	float SpecularSharpness;
	int4 Texture_Index; // X - diffuse map, Y - Specular map, Z - Bump Map, W - Mask Map
	float4 Roulette; // X - Diffuse ammount, Y - Specular, Z - Mirror scattering, W - Fresnell scattering
	float3 Emissive; // Emissive component for lights
};

struct VolumeMaterial {
	float3 Extinction;
	float3 ScatteringAlbedo;
	float3 G;
};

// Vertex Data
struct Vertex
{
	// Position
	float3 P;
	// Normal
	float3 N;
	// Texture coordinates
	float2 C;
	// Tangent Vector
	float3 T;
	// Binormal Vector
	float3 B;
};

#ifndef pi
#define pi 3.1415926535897932384626433832795
#define piOverTwo 1.5707963267948966192313216916398
#define inverseOfPi 0.31830988618379067153776752674503
#define inverseOfTwoPi 0.15915494309189533576888376337251
#define two_pi 6.283185307179586476925286766559
#endif

// Transform every surfel component with a specific transform
// Valid for isometric transformation since directions are transformed
// using a simple multiplication operation.
Vertex Transform(Vertex surfel, float4x3 transform) {
	Vertex v = {
		mul(float4(surfel.P, 1), transform),
		mul(float4(surfel.N, 0), transform),
		surfel.C,
		mul(float4(surfel.T, 0), transform),
		mul(float4(surfel.B, 0), transform) };
	return v;
}

#endif // !DEFINITIONS_H