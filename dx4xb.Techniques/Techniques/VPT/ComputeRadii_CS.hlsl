Texture3D<float>		Grid		: register(t0);

RWTexture3D<float4>		Parameters	: register(u0);
RWTexture3D<int>		Radii		: register(u1);

sampler LinearSampler : register(s0);

cbuffer ThresholdInfo : register(b0) {
	float RadiusThreshold;
	int StartLevel;
};

#include "..\Tools\Parameters.h"

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	int3 dim;
	Radii.GetDimensions(dim.x, dim.y, dim.z);

	float3 coordinates = (DTid + 0.5) / dim;

	int max_levels = 8;

	//if (StartLevel == 0)
	{
		Radii[DTid] = 1 << StartLevel;
		Parameters[DTid] = float4(0, 0, 0, Grid.mips[StartLevel][DTid]);
	}

	for (int level = max(1, StartLevel); level < max_levels; level++)
	{
		float error = 0;
		float4 Q = float4(3, 3, 3, 1);
		float4 parameters = 0;
		for (int z = -1; z <= 1; z++)
			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					float3 pos = float3(x, y, z) * 0.66666;
					float3 coord = (DTid + pos * (1 << (level - StartLevel)) + 0.5) / dim;
					float Y = Grid.SampleLevel(LinearSampler, coord, level);
					float4 phi = float4(pos.x, pos.y, pos.z, 1);
					parameters += phi * Y * Q;
				}
		parameters /= 27;

		for (int z = -1; z <= 1; z++)
			for (int y = -1; y <= 1; y++)
				for (int x = -1; x <= 1; x++)
				{
					float3 pos = float3(x, y, z) * 0.66666;
					float3 coord = (DTid + pos * (1 << (level - StartLevel)) + 0.5) / dim;
					float Y = Grid.SampleLevel(LinearSampler, coord, level);
					float4 phi = float4(pos.x, pos.y, pos.z, 1);
					float estimatedValue = dot(parameters, phi);
					error += pow((Y - estimatedValue), 2) * level / max(0.0000001, Y);
				}
		error /= 27;

		if (level > StartLevel && error > RadiusThreshold)
			break;

		Radii[DTid] = 1 << level;
		Parameters[DTid] = float4(parameters.xyz/max(0.000001, parameters.w), parameters.w);
	}
}