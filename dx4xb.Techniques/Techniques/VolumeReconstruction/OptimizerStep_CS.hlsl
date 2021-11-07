StructuredBuffer<uint> Gradients : register(t0);
RWStructuredBuffer<float> Parameters : register(u0);

cbuffer OptimizationParameters : register(b0)
{
	float lr;
};

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	Parameters[DTid.x] -= lr * asfloat(Gradients[DTid.x]);
}