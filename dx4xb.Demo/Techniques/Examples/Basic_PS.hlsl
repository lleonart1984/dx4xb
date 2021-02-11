struct PS_IN {
	float4 Proj : SV_POSITION;
	float3 P : POSITION; // World-Space Position
	float3 N : NORMAL; // World-Space Normal
	float2 C : TEXCOORD;
};

Texture2D<float4> Texture : register(t0);

sampler Samp : register(s0);

float4 main(PS_IN pIn) : SV_TARGET
{
	//return float4(1,1,1,1);// max(0, dot(pIn.N, normalize(float3(1,1,1))))* Texture.Sample(Samp, pIn.C);
	return float4(pIn.N, 1);
}