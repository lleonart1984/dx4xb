Texture3D<float> Grid : register(t0);
Texture3D<float3> Gradients : register(t1);
RWTexture3D<float4> Parameters : register(u0);

cbuffer LevelInfo : register(b0) {
	int Level;
};

#include "..\Tools\Parameters.h"

float Phi_0(float t) {
	return 1;
}

float Phi_1(float t) {
	return t;
}

float Phi_2(float t) {
	return 0;// (5 * t * t * t - 3 * t) * 0.5;
}


[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float3 grad = Gradients[DTid];
	grad = length(grad) > 0.001 ? normalize(grad) : float3(0, 0, 1);
	float3 Q = float3(1, 3, 7);
	int size = VOXEL_SIZE * (1 << Level);
	float totalWeight = 0;

	float3 parameters = 0;
	for (int z = 0; z < size; z++)
		for (int y = 0; y < size; y++)
			for (int x = 0; x < size; x++)
			{
				float3 pos = 2 * ((float3(x, y, z) + 0.5) / size) - 1;
				//if (length(pos) <= 1) 
				{
					float Y = Grid[DTid * size + int3(x, y, z)];
					float t = dot(pos, grad);
					float3 phi = float3(Phi_0(t), Phi_1(t), Phi_2(t));
					float w = 1;// pow(abs(t), 2);
					parameters += phi * Y * Q * w;
					totalWeight += w;
				}
			}

	parameters /= totalWeight;

	float error = 0;
	for (int z = 0; z < size; z++)
		for (int y = 0; y < size; y++)
			for (int x = 0; x < size; x++)
			{
				float3 pos = 2 * ((float3(x, y, z) + 0.5) / size) - 1;
				//if (length(pos) <= 1) 
				{
					float Y = Grid[DTid * size + int3(x, y, z)];
					float t = dot(pos, grad);
					float3 phi = float3(Phi_0(t), Phi_1(t), Phi_2(t));
					float w = 1;// pow(abs(t), 2);
					error += size * pow((dot(phi, parameters) - Y), 2) * w;
				}
			}

	error /= totalWeight;

	Parameters[DTid] = float4(parameters, error);
}