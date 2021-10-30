#include "..\Tools\Definitions.h"

Texture3D<float4> Grid	: register(t0, space0); // Grid used in this pathtracer (value, average, radii)
Texture3D<float> Deep	: register(t1, space0); // Grid used in this pathtracer (value, average, radii)

sampler VolSampler : register(s0);

RWTexture2D<float4> Output						: register(u0, space0); // Final Output image from the vis


cbuffer Camera : register(b0) {
	float4x4 FromProjectionToWorld;
};

cbuffer VISInfo  : register(b1) {
	float Slice;
	bool ShowComplexity;
}

void GetGridBox(out float3 minim, out float3 maxim) {
	int w, h, d;
	Grid.GetDimensions(w, h, d);
	float maxDim = max(w, max(h, d));
	maxim = float3(w, h, d) / maxDim * 0.5;
	minim = -maxim;
}

float3 GetColor(float v) {
	float3 colors[10];

	/*if (v % 1 > 0.9)
		return float3(1, 1, 1);*/

	/*colors[0] = float3(0, 0, 1);
	colors[1] = float3(0, 1, 1);
	colors[2] = float3(0, 1, 0);
	colors[3] = float3(1, 1, 0);
	colors[4] = float3(1, 0, 0);
	colors[5] = float3(1, 0, 1);
	colors[6] = float3(1, 1, 1);*/
	colors[7] = 1.0/255*float3(253, 231, 37);
	colors[6] = 1.0/255*float3(160, 218, 57);
	colors[5] = 1.0/255*float3(74, 193, 109);
	colors[4] = 1.0/255*float3(31, 161, 135);
	colors[3] = 1.0/255*float3(39, 127, 142);
	colors[2] = 1.0/255*float3(54, 92, 141);
	colors[1] = 1.0/255*float3(70, 50, 126);
	colors[0] = 1.0/255*float3(68, 1, 84);
	if (v > 7) v = 7;
	return lerp(colors[(int)v], colors[(int)v + 1], v % 1);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 dim;
	Output.GetDimensions(dim.x, dim.y);

	float4 ndcP = float4(2 * (DTid.xy + 0.5) / dim - 1, 0, 1);
	ndcP.y *= -1;
	float4 ndcT = ndcP + float4(0, 0, 1, 0);
	float4 ndcSC = float4(0, 0, 0, 1);
	float4 ndcST = float4(0, 0, 1, 1);

	float4 viewP = mul(ndcP, FromProjectionToWorld);
	viewP.xyz /= viewP.w;
	float4 viewT = mul(ndcT, FromProjectionToWorld);
	viewT.xyz /= viewT.w;

	ndcSC = mul(ndcSC, FromProjectionToWorld);
	ndcSC.xyz /= ndcSC.w;
	ndcST = mul(ndcST, FromProjectionToWorld);
	ndcST.xyz /= ndcST.w;

	float3 cameraDir = normalize(ndcST.xyz - ndcSC.xyz);

	float3 O = viewP.xyz;
	float3 D = normalize(viewT.xyz - viewP.xyz);

	float3 minim, maxim;
	GetGridBox(minim, maxim);

	float3 color = 0;
	float alpha = 0;


		float3 P = float3(0, 0, Slice);
		float3 N = float3(0, 0, -1);// normalize(-cameraDir);
		float a = dot(P - O, N) / dot(N, D);

		float3 samplePosition = O + D * a;

		float3x3 transform = float3x3(
			1, 0, 0,
			0, 1, 0,
			0.5, 0.5, 0.25
			);

		//samplePosition = mul(samplePosition, transform);

		float3 sampleCoord = (samplePosition - minim) / (maxim - minim);

		float3 backColor = 0;

		if (all(sampleCoord >= 0) && all(sampleCoord <= 1))
			backColor = float3(0.4, 0.4, 1) * 0.1;

		float currentAlpha = 5.0 / 6;
		float3 currentColor = backColor;

		if (ShowComplexity)
		{
			currentColor += GetColor(Deep.SampleLevel(VolSampler, sampleCoord, 0).x * 30);
		}
		else {
			currentColor += GetColor(Grid.SampleLevel(VolSampler, sampleCoord, 0).z);
			//currentColor += GetColor(Grid.SampleLevel(VolSampler, sampleCoord, 0).x * 6);
		}

		color += currentColor;// (1 - alpha)* currentAlpha* currentColor;
		alpha *= (1 - currentAlpha);
	

	Output[DTid.xy] = float4(float3(1, 1, 1) * color, 1);
}