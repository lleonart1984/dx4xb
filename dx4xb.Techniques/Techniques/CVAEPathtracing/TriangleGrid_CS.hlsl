/// GRID VOXELIZATION

// Creates a grid of linked list. Each cell contains a linked list to all triangles intersecting

#include "../Tools/Definitions.h"

// Geometry description (Only geometric information needed)
StructuredBuffer<Vertex> Vertices	: register(t0);
StructuredBuffer<int> Indices		: register(t1);

// Triangles indices for each node of the linked lists
RWStructuredBuffer<int> TriangleIndices : register(u0); // Linked list values (references to the triangles)
// Grid of all linked lists' head
// Per cell linked list head. This buffer should be filled with -1 (Null references) before starting.
RWTexture3D<int> Head : register(u1);
// For each node represents the next node
RWStructuredBuffer<int> Next : register(u2); // Per linked lists next references....
// Buffer used to allocate node references with interlocked operation
RWTexture1D<int> Malloc : register(u3); // incrementer buffer

cbuffer GridTransform : register(b0) {
	float4x4 FromGeometryToGrid;
}

// Converts a geometry space position into a grid space (0,0,0) - (Size, Size, Size)
float3 FromPositionToCell(float3 P) {
	return mul(float4(P,1), FromGeometryToGrid).xyz;
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint NumberOfVertices, stride;
	Indices.GetDimensions(NumberOfVertices, stride);

	if (DTid.x >= NumberOfVertices/3)
		return;

	float3 c1 = FromPositionToCell(Vertices[Indices[DTid.x * 3 + 0]].P);
	float3 c2 = FromPositionToCell(Vertices[Indices[DTid.x * 3 + 1]].P);
	float3 c3 = FromPositionToCell(Vertices[Indices[DTid.x * 3 + 2]].P);

	float3 P = c1;
	// this is a seudo normal used to determine plane side of cell corners.
	float3 N = cross(c3 - c1, c2 - c1);

	// Determining range of cells that cover the triangle.
	int3 maxCell = max(c1, max(c2, c3));// +0.00001;
	int3 minCell = min(c1, min(c2, c3));// -0.00001;

	// 8 evals for the initial cell's corner against the triangle plane.
	float2x4 evals = float2x4(
		dot(minCell + float3(0, 0, 0) - P, N), dot(minCell + float3(1, 0, 0) - P, N), dot(minCell + float3(0, 1, 0) - P, N), dot(minCell + float3(1, 1, 0) - P, N),
		dot(minCell + float3(0, 0, 1) - P, N), dot(minCell + float3(1, 0, 1) - P, N), dot(minCell + float3(0, 1, 1) - P, N), dot(minCell + float3(1, 1, 1) - P, N)
		);

	for (int cz = minCell.z; cz <= maxCell.z; cz++)
		for (int cy = minCell.y; cy <= maxCell.y; cy++)
			for (int cx = minCell.x; cx <= maxCell.x; cx++)
			{
				int3 currentCell = int3(cx, cy, cz);
				float2x4 currentEvals = evals + dot(N, currentCell - minCell);
				// Intersection occurs if there is a case of positive evaluation and negative evaluation.
				if (!all(currentEvals <= 0) && !all(currentEvals >= 0)) // cell intersects triangle
				{
					int currentReference;
					InterlockedAdd(Malloc[0], 1, currentReference);
					TriangleIndices[currentReference] = DTid.x;
					InterlockedExchange(Head[currentCell], currentReference, Next[currentReference]);
				}
			}
}