Texture3D<float> Grid : register(t0);

RWTexture3D<float3> Gradients : register(u0);

#include "..\Tools\Parameters.h"

float3 Gradient(int3 pos) {
	float3 gradient = 0;

	for (int z = -1; z <= 1; z++)
		for (int y = -1; y <= 1; y++)
			for (int x = -1; x <= 1; x++)
				gradient += float3(x, y, z) * Grid[pos + int3(x, y, z)] ;

	return gradient;

	/*return float3(
		(Grid[pos + int3(1, 0, 0)] - Grid[pos - int3(1, 0, 0)]) * 0.5,
		(Grid[pos + int3(0, 1, 0)] - Grid[pos - int3(0, 1, 0)]) * 0.5,
		(Grid[pos + int3(0, 0, 1)] - Grid[pos - int3(0, 0, 1)]) * 0.5
		);*/
}

float3 safe_normalize(float3 n) {
	if (dot(n, n) < 0.00000001)
		return normalize(float3(1,1,1));
	return normalize(n);
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float3 gradient = 0;
	for (int z = 0; z < VOXEL_SIZE; z++)
		for (int y = 0; y < VOXEL_SIZE; y++)
			for (int x = 0; x < VOXEL_SIZE; x++)
				gradient += (Gradient(DTid * VOXEL_SIZE + int3(x, y, z)));
	//gradient = gradient / (VOXEL_SIZE * VOXEL_SIZE);
	
	/*float3 gradient = 0;
	float totalDensity = 0;
	for (int z = 0; z < VOXEL_SIZE; z++)
		for (int y = 0; y < VOXEL_SIZE; y++)
			for (int x = 0; x < VOXEL_SIZE; x++)
			{
				gradient += pow(Grid[DTid * VOXEL_SIZE + int3(x, y, z)], 0.5) * (2 * (float3(x, y, z) + 0.5) / VOXEL_SIZE - 1);
				totalDensity += Grid[DTid * VOXEL_SIZE + int3(x, y, z)];
			}
	gradient /= max(0.0001, totalDensity);*/

	Gradients[DTid] = safe_normalize(gradient);
}