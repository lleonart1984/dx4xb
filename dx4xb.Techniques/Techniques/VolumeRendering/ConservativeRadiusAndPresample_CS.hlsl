Texture3D<float4> Grid : register(t0);
Texture3D<int> Radii : register(t1);
sampler VolSampler : register(s0);
RWTexture3D<float3> HGrid : register(u0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int3 dim;
	Grid.GetDimensions(dim.x, dim.y, dim.z);

	int3 p = int3(DTid.x % dim.x, (DTid.x / dim.x) % dim.y, (DTid.x / (dim.x * dim.y)) % dim.z);

	float3 sampleCoord = (p + 0.5) / float3(dim);

	int level = Radii[p];
	
	/*for (int dz = -1; dz <= 1; dz++)
		for (int dy = -1; dy <= 1; dy++)
			for (int dx = -1; dx <= 1; dx++)
			{
				int3 adj = max(0, min(dim - 1, p + int3(dx, dy, dz)));
				level = min(level, Radii[adj]);
			}*/

	//while (level > 0) {
	//	bool isOk = true;
	//	for (int dz = -1; dz <= 1; dz++)
	//		for (int dy = -1; dy <= 1; dy++)
	//			for (int dx = -1; dx <= 1; dx++)
	//			{
	//				int3 adj = max(0, min(dim - 1, p + int3(dx, dy, dz) * (1 << (level - 0))));
	//				if (level > Radii[adj])
	//					isOk = false;
	//			}
	//	if (isOk)
	//		break;
	//	level--; // no conservative radius found, reduce level.
	//}

	HGrid[p].x = Grid[p].x; // original value at level 0
	HGrid[p].y = Grid.SampleLevel(VolSampler, sampleCoord, level).x; // average value
	HGrid[p].z = level; //log(2R)
}