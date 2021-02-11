Texture2D<float4> tex : register(t0);

sampler smp : register(s0);

float4 main(float4 pos : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET
{
	return tex.SampleLevel(smp, texCoord, 0);
}