Texture3D<float4> Grid : register(t0);
sampler VolSampler : register(s0);
RWTexture3D<float> Deep : register(u0);

#include "..\Tools\Randoms.h"

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);


	int3 p = int3(DTid.x % dim.x, (DTid.x / dim.x) % dim.y, (DTid.x / (dim.x * dim.y)) % dim.z);
	StartRandomSeedForRay(dim.xy, 1, p.xy, p.z, dim.z);

	float3 sampleCoord = (p + 0.5) / float3(dim);

	int maxDim = max(dim.x, max(dim.y, dim.z));

	float deep = 1;
	for (int d = 0; d < 10; d++) {
		float3 sampleDir = randomDirection(float3(0, 0, 1));
		float total = 0;
		for (int i = 0; i < log2(maxDim); i++)
		{
			float3 smpPos = sampleCoord + sampleDir * (1 << i) / maxDim;
			total += Grid.SampleLevel(VolSampler, smpPos, i).x * (1 << i);
		}
		deep = min(deep, total / maxDim);
	}

	Deep[p] = deep;
}