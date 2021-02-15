#include "../Pathtracing/PathtracingCommon_RT.h"

struct GridInfo {
	// Index of the base geometry (grid).
	int GridIndex;
	// Transform from the world space to the grid.
	// Considers Instance Transform, Geometry Transform and Grid transform.
	float4x4 FromWorldToGrid;
	// Scaling factor to convert distances from grid (cell is unit) to world.
	float FromGridToWorldScaling;
};
StructuredBuffer<GridInfo> GridInfos : register(t1);
Texture3D<float> DistanceField[100] : register(t2);

/// Query the distance field grid.
float MaximalRadius(float3 P, int object) {

	GridInfo info = GridInfos[object];
	float3 positionInGrid = mul(float4(P, 1), info.FromWorldToGrid).xyz;
	float radius = DistanceField[info.GridIndex][positionInGrid];

	if (radius < 0) // no empty cell
		return 0;

	float3 distToMinCorner = positionInGrid % 1;
	float3 m = min(distToMinCorner, 1 - distToMinCorner);
	float minDistanceToCellBorder = min(m.x, min(m.y, m.z));
	//float safeDistanceInGridSpace = radius;
	float safeDistanceInGridSpace = minDistanceToCellBorder + radius;
	return safeDistanceInGridSpace * info.FromGridToWorldScaling;
}

