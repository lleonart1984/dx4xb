Texture3D<float4> Grid : register(t0);
sampler VolSampler : register(s0);
RWTexture3D<int> Radii : register(u0);

bool Check(float3 c, int level) {

	float4 smp = Grid.SampleLevel(VolSampler, c, level);

	float max = smp.w;
	float min = smp.z;
	float sqr = smp.y;
	float ave = smp.x;

	float var = level == 0 ? 0 : sqr - ave * ave;

	//return (max - ave) <= 0.0; // skip only really homogeneous space
	//return false; // no skip
	//return (max - ave) <= 0.5;// max * 0.01;
	return sqrt(var) <= 0.4 * ave;// max * 0.01;
}

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);

	int3 p = int3(DTid.x % dim.x, (DTid.x / dim.x) % dim.y, (DTid.x / (dim.x * dim.y)) % dim.z);

	float3 sampleCoord = (p + 0.5)/float3(dim);

	int level = 0;
	
	while (all(dim > 0))
	{
		if (!Check(sampleCoord, level))
			break;
		
		level += 1;
		dim /= 2;
		//sampleCoord /= 2;
	}

	Radii[p] = max(0, level - 1);
}