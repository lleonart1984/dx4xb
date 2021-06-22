// CB used to count passes of the PT and to know if the output image is a complexity image or the final color.
cbuffer AccumulativeInfo : register(b0) {
	uint NumberOfPasses;
	uint ShowComplexity;
	float PathtracingRatio;
	uint AnimatedFrame;
}

Texture2D<uint> Complexity						: register(t0, space0); // Complexity buffer for debuging

// Output textures
RWTexture2D<float4> Output						: register(u0, space0); // Final Output image from the ray-trace


#include "..\Tools\CommonComplexity.h"


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	Output[DTid.xy] = float4(GetColor((int)ceil(Complexity[DTid.xy] / (NumberOfPasses + 1.0))), 1);
}