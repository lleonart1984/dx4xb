struct VS_IN {
	float3 P : POSITION;
	float3 N : NORMAL;
	float2 C : TEXCOORD;
};

struct VS_OUT {
	float4 Proj : SV_POSITION;
	float3 P : POSITION; // World-Space Position
	float3 N : NORMAL; // World-Space Normal
	float2 C : TEXCOORD;
};

StructuredBuffer<float4x4> InstanceTransforms : register(t0);
StructuredBuffer<float4x3> GeometryTransforms : register(t1);

cbuffer Camera : register(b0) {
	// no necessary row_major here, float4x4 in CA4G is col-order as default in HLSL
	float4x4 Projection;
	float4x4 View;
};

cbuffer ObjectInfo : register(b1) {
	// Simple constants in the root signature.
	int InstanceIndex;
	int TransformIndex;
	int MaterialIndex;
};

VS_OUT main(VS_IN vIn)
{
	VS_OUT Out = (VS_OUT)0;

	float4 P = float4(vIn.P, 1);
	float4 N = float4(vIn.N, 0);

	if (TransformIndex >= 0)
	{
		P = float4(mul(P, GeometryTransforms[TransformIndex]), 1);
		N = float4(mul(N, GeometryTransforms[TransformIndex]), 0);
	}

	P = mul(P, InstanceTransforms[InstanceIndex]);
	N = mul(N, InstanceTransforms[InstanceIndex]);

	Out.P = P;
	Out.N = normalize(N);
	Out.C = vIn.C;
	Out.Proj = mul(mul(P, View), Projection);

	return Out;
}