#ifndef ELEMENT_TYPE
#define ELEMENT_TYPE float
#endif

#ifndef POOL_SIZE
#define POOL_SIZE 2
#endif

Texture3D<ELEMENT_TYPE> Src : register(t0);
RWTexture3D<ELEMENT_TYPE> Dst : register(u0);

ELEMENT_TYPE Pool(ELEMENT_TYPE elements[POOL_SIZE * POOL_SIZE * POOL_SIZE]);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	ELEMENT_TYPE elements[POOL_SIZE * POOL_SIZE * POOL_SIZE];
	int count = 0;
	for (int i = 0; i < POOL_SIZE; i++)
		for (int j = 0; j < POOL_SIZE; j++)
			for (int k = 0; k < POOL_SIZE; k++)
				elements[count++] = Src[DTid * POOL_SIZE + int3(k, j, i)];
	Dst[DTid] = Pool(elements);
}