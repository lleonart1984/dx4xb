RWTexture2D<uint> Counting : register(u0);

void main(float4 proj : SV_POSITION)
{
	InterlockedAdd(Counting[proj.xy], 1);
}