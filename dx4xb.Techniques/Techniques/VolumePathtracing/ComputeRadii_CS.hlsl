Texture3D<float> Grid : register(t0);
sampler GridSampler : register(s0);

Texture3D<float> Majorant : register(t1);
sampler MaxSampler : register(s1);

Texture3D<float> Minorant : register(t2);
sampler MinSampler : register(s2);

Texture3D<float> Average : register(t3);

RWTexture3D<int> Radii : register(u0);

bool Check(float3 c, int level) {

	float max = Majorant.SampleLevel(MaxSampler, c, level-1);
	float min = Minorant.SampleLevel(MinSampler, c, level-1);
	float ave = Average.SampleLevel(GridSampler, c, level-1);

	return (max - ave) <= 0.1*max; // skip only really homogeneous space
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);

	float3 sampleCoord = (DTid + 0.5) / float3(dim);

	int level = 1;

	while (all(dim > 1))
	{
		if (!Check(sampleCoord, level))
			break;

		level += 1;
		dim = dim / 2;
	}

	Radii[DTid] = max(0, level - 1);
}