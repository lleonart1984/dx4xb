Texture3D<float>		Grid		: register(t0);
Texture3D<float3>		Gradients	: register(t1);

RWTexture3D<int>		Radii		: register(u0);
RWTexture3D<float4>		Parameters	: register(u1);

sampler GridSampler : register(s0);
sampler GradientSampler : register(s1);

#include "..\Tools\Parameters.h"

float3 safe_normalize(float3 n) {
	if (length(n) < 0.0000001)
		return float3(0, 0, 1);
	return normalize(n);
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);

	Radii[DTid] = 1;
	Parameters[DTid] = float4(0, 0, 0, Grid[DTid]);
	
	float3 coordinates = (DTid + 0.5) / dim;

	int max_levels = 8;

	for (int level = 1; level < max_levels; level++)
	{
		float3 gradient = safe_normalize(Gradients.SampleLevel(GradientSampler, coordinates, level));
		float error = 0;
		float2 parameters = 0;
		for (int z = -1; z <= 1; z++)
			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					float3 smpCoord = (DTid + int3(x, y, z) * (1 << level) + 0.5) / dim;
					float value = Grid.SampleLevel(GridSampler, smpCoord, level);
					float t = dot(gradient, float3(x, y, z));
					parameters += float2(1, t) * value * float2(1, 3);
				}
		parameters /= 27;

		for (int z = -1; z <= 1; z++)
			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					float3 smpCoord = (DTid + int3(x, y, z) * (1 << level) + 0.5) / dim;
					float value = Grid.SampleLevel(GridSampler, smpCoord, level);
					float t = dot(gradient, float3(x, y, z));
					float estimatedValue = parameters.x + parameters.y * t;
					error += pow((value - estimatedValue), 2) * level / max(0.0000001, value);
				}
		error /= 27;

		if (error <= 0.01) {
			Radii[DTid] = 1 << level;
			Parameters[DTid] = float4(gradient * parameters.y / max(0.0001, parameters.x), parameters.x);
		}
		else
			break;
	}
}