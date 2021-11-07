/*
Reset gradient to 0.
*/

RWStructuredBuffer<uint> Gradient : register(u0);

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	Gradient[DTid.x] = 0;
}