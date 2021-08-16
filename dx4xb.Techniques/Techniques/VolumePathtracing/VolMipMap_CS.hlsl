Texture3D<float> Src_Majorant : register(t0);
Texture3D<float> Src_Minorant : register(t1);
Texture3D<float> Src_Average  : register(t2);

RWTexture3D<float> Dst_Majorant : register(u0);
RWTexture3D<float> Dst_Minorant : register(u1);
RWTexture3D<float> Dst_Average  : register(u2);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	Dst_Majorant[DTid] =
		max(
			max(
				max(Src_Majorant[DTid * 2 + int3(0, 0, 0)],
					Src_Majorant[DTid * 2 + int3(0, 0, 1)]),
				max(Src_Majorant[DTid * 2 + int3(0, 1, 0)],
					Src_Majorant[DTid * 2 + int3(0, 1, 1)])),
			max(
				max(Src_Majorant[DTid * 2 + int3(1, 0, 0)],
					Src_Majorant[DTid * 2 + int3(1, 0, 1)]),
				max(Src_Majorant[DTid * 2 + int3(1, 1, 0)],
					Src_Majorant[DTid * 2 + int3(1, 1, 1)])));
	Dst_Minorant[DTid] =
		min(
			min(
				min(Src_Minorant[DTid * 2 + int3(0, 0, 0)],
					Src_Minorant[DTid * 2 + int3(0, 0, 1)]),
				min(Src_Minorant[DTid * 2 + int3(0, 1, 0)],
					Src_Minorant[DTid * 2 + int3(0, 1, 1)])),
			min(
				min(Src_Minorant[DTid * 2 + int3(1, 0, 0)],
					Src_Minorant[DTid * 2 + int3(1, 0, 1)]),
				min(Src_Minorant[DTid * 2 + int3(1, 1, 0)],
					Src_Minorant[DTid * 2 + int3(1, 1, 1)])));
	Dst_Average[DTid] = (
		Src_Average[DTid * 2 + int3(0, 0, 0)] +
		Src_Average[DTid * 2 + int3(0, 0, 1)] +
		Src_Average[DTid * 2 + int3(0, 1, 0)] +
		Src_Average[DTid * 2 + int3(0, 1, 1)] +
		Src_Average[DTid * 2 + int3(1, 0, 0)] +
		Src_Average[DTid * 2 + int3(1, 0, 1)] +
		Src_Average[DTid * 2 + int3(1, 1, 0)] +
		Src_Average[DTid * 2 + int3(1, 1, 1)]
		) / 8.0;
}