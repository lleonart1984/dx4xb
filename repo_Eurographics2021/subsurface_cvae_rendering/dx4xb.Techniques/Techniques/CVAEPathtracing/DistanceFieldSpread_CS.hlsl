/// For a specific level, use the conservative distance in 
/// 3x3x3-1 adjacent blocks of size 3^level to determine a conservative distance 
/// in a 1x1x1 block of size 3^{level+1}.

/// Grid with source values
Texture3D<float>		GridSrc : register(t0);
/// Grid constructed with the new values for each cell
RWTexture3D<float>		GridDst : register(u0);

cbuffer LevelInfo : register(b0) {
	int Level;
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint Size, height, depth;
	GridSrc.GetDimensions(Size, height, depth);

	/// for each cell consider the distances in adjacent cells at specific distance (depending on the level)
	/// If all distances are greater than the required distance to spread, the new distance is updated.

	int3 currentCell = int3(DTid.x % Size, DTid.x / Size % Size, DTid.x / (Size * Size));

	int Radius = (int)round(pow(3, Level));
	float RequiredDistance = (Radius - 1) * 0.5;

	float minDistance = 10000;
	for (int bz = -1; bz <= 1; bz++)
		for (int by = -1; by <= 1; by++)
			for (int bx = -1; bx <= 1; bx++)
			{
				int3 adjCell = clamp(int3(bx, by, bz) * (Radius)+currentCell, 0, Size - 1);
				minDistance = min(minDistance, GridSrc[adjCell]);
			}

	if (minDistance >= RequiredDistance)
	{ // can spread
		GridDst[currentCell] = 2 * RequiredDistance + 1 + minDistance;
	}
	else
	{ // can not enlarge with current info
		GridDst[currentCell] = GridSrc[currentCell];
	}
}
