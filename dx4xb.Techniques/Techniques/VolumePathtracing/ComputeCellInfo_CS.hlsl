#include "..\Tools\Parameters.h"

Texture3D<float> Grid : register(t0);
RWTexture3D<float> Majorant : register(u0);
RWTexture3D<float> Minorant : register(u1);
RWTexture3D<float> Average  : register(u2);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float ma = 0;
	float mi = 10000;
	float ave = 0;
	for (int z = 0; z < SV_SIZE; z++)
		for (int y = 0; y < SV_SIZE; y++)
			for (int x = 0; x < SV_SIZE; x++)
			{
				float value = Grid[DTid * SV_SIZE + uint3(x, y, z)];
				ma = max(ma, value);
				mi = min(mi, value);
				ave += value;
			}
	Majorant[DTid] = ma;
	Minorant[DTid] = mi;
	Average[DTid] = ave / (SV_SIZE * SV_SIZE * SV_SIZE);
}