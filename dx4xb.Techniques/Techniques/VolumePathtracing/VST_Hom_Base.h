#include "VST_Base.h"

Texture3D<float2> HSphereGrid : register(t1, space0); // Homogenous Impostor Grid used in this pathtracer (average density and radius)

float UnitarySphereHomogeneousTransmittance(float density, float g, float albedo, inout float3 w, out float3 x);

float SphereTransmittance(inout float3 x, inout float3 w) {
	complexity++;
	float3 coordinate = (x - Grid_Min) / (Grid_Max - Grid_Min);
	float2 ar = HSphereGrid.SampleGrad(GridSampler, coordinate, 0, 0).xy;

	float density = GetComponent(Extinction);
	float g = GetComponent(G);
	float albedo = GetComponent(ScatteringAlbedo);

	ar.y *= 1 + log(1 + scatters * 0.01);

	float effective_density = ar.x * ar.y * density;

	float3 xs;
	float T = UnitarySphereHomogeneousTransmittance(effective_density, g, albedo, w, xs);
	x += xs * ar.y; // scale with radius
	return T;
}

