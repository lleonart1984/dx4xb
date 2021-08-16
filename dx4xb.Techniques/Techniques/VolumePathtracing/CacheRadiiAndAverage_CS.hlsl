
Texture3D<float> Grid			: register(t0);
Texture3D<float> Average		: register(t1);
Texture3D<int> Radii			: register(t2);

sampler GridSampler : register(s0);

RWTexture3D<float2> SphereGrid	: register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);
	int maxDim = max(dim.x, max(dim.y, dim.z));

	float cellSize = 0.5 / maxDim;

	float3 sampleCoord = (DTid + 0.5) / float3(dim);

	int level = Radii[DTid];

	float average = level <= 0 ? Grid[DTid] : Average.SampleLevel(GridSampler, sampleCoord, level - 1);
	float r = (1 << level) * cellSize * (average > 0.0 ? 2 : 1);

	SphereGrid[DTid] = float2(average, r);
}