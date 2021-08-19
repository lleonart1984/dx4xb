Texture3D<float> Grid			: register(t0);
RWTexture3D<float4> Parameters	: register(u0);
RWTexture3D<float> Errors		: register(u1);

sampler LinearSampler :  register(s0);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float4 Q = float4(3/8, 3/8, 3/8, 1);
	
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);

	float4 parameters = 0;
	for (int z = -1; z <= 1; z++)
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= -1; x++)
			{
				float3 pos = float3(x, y, z) * 0.66666;
				float3 coord = (DTid + 0.5 + pos) / dim;
				float Y = Grid.SampleLevel(LinearSampler, coord, 0);
				float4 phi = float4(pos.x, pos.y, pos.z, 1);
				parameters += phi * Y * Q;
			}

	parameters /= 27;

	float error = 0;
	for (int z = -1; z <= 1; z++)
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= -1; x++)
			{
				float3 pos = float3(x, y, z) * 0.66666;
				float3 coord = (DTid + 0.5 + pos) / dim;
				float Y = Grid.SampleLevel(LinearSampler, coord, 0);
				float4 phi = float4(pos.x, pos.y, pos.z, 1);
				error += pow(Y - dot(phi, parameters), 2);
			}

	error /= 27;

	Parameters[DTid] = parameters;
	Errors[DTid] = error;
}