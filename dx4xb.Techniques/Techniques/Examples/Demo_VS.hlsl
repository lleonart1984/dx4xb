void main(float3 pos : POSITION, out float4 outPos : SV_POSITION, out float2 texCoord : TEXCOORD0)
{
	outPos = float4(pos * .8, 1);
	texCoord = pos.xy * 0.5 + 0.5;
}