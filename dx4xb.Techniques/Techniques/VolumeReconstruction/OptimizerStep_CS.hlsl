StructuredBuffer<int> Gradients : register(t0);
RWStructuredBuffer<float> Parameters : register(u0);

cbuffer OptimizationParameters : register(b0)
{
	float lr;
};

float int_to_float(int value) {
	return value / (float)(1 << 10);
}

int float_to_int(float value) {
	return (int)(value * (1 << 10));
}

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (Gradients[DTid.x])
	{
		Parameters[DTid.x] = Parameters[DTid.x] - lr * int_to_float(Gradients[DTid.x]);
	}
}