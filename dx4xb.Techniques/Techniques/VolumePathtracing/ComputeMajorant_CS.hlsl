#include "..\Tools\Parameters.h"

Texture3D<float> Grid : register(t0);
RWTexture3D<float> Majorant : register(u0);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float m = 0;
	for (int z = 0; z < SV_SIZE; z++)
		for (int y = 0; y < SV_SIZE; y++)
			for (int x = 0; x < SV_SIZE; x++)
				m = max(m, Grid[DTid * SV_SIZE + uint3(x, y, z)]);
	Majorant[DTid] = m;
}