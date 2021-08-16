#include "BasePooling3D.h"

ELEMENT_TYPE Pool(ELEMENT_TYPE elements[POOL_SIZE * POOL_SIZE * POOL_SIZE])
{
	ELEMENT_TYPE minimum = elements[0];
	for (int i = 1; i < POOL_SIZE * POOL_SIZE * POOL_SIZE; i++)
		minimum = min(minimum, elements[i]);
	return minimum;
}