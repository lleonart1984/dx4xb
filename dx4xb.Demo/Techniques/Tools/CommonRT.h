
/// Common header to any library that will use the scene retained as follow.
/// Provides functions to get intersection information
/// Augment material and surfel with texture mapping (including bump mapping)

#include "Definitions.h"

StructuredBuffer<Vertex> VertexBuffer			: register(t0, space1);	// All vertices in scene.
StructuredBuffer<int> IndexBuffer				: register(t1, space1); // Indices of geometries in scene.
StructuredBuffer<float4x3> Transforms			: register(t2, space1); // All materials.
StructuredBuffer<Material> Materials			: register(t3, space1); // All materials.
StructuredBuffer<VolumeMaterial> VolMaterials	: register(t4, space1); // All materials.
Texture2D<float4> Textures[500]					: register(t5, space1); // All textures used

RWTexture2D<float4> Output						: register(u0, space1); // Final Output image from the ray-trace

struct ObjInfo {
	int StartTriangle;
	int VertexOffset;
	int TransformIndex;
	int MaterialIndex;
};
// Locals for hit groups (fresnel and lambert)
ConstantBuffer<ObjInfo> ObjectInfo	: register(b0, space1);

// Used for texture mapping
SamplerState gSmp : register(s0, space1);

// Given a surfel will modify the normal with texture maps, using
// Bump mapping and masking textures.
// Material info is updated as well.
void AugmentHitInfoWithTextureMapping(inout Vertex surfel, inout Material material, float ddx, float ddy) {
	float4 DiffTex = material.Texture_Index.x >= 0 ? Textures[material.Texture_Index.x].SampleGrad(gSmp, surfel.C, ddx, ddy) : float4(1, 1, 1, 1);
	float3 SpecularTex = material.Texture_Index.y >= 0 ? Textures[material.Texture_Index.y].SampleGrad(gSmp, surfel.C, ddx, ddy).xyz : material.Specular;
	float3 BumpTex = material.Texture_Index.z >= 0 ? Textures[material.Texture_Index.z].SampleGrad(gSmp, surfel.C, ddx, ddy).xyz : float3(0.5, 0.5, 1);
	float3 MaskTex = material.Texture_Index.w >= 0 ? Textures[material.Texture_Index.w].SampleGrad(gSmp, surfel.C, ddx, ddy).xyz : 1;

	float3x3 TangentToWorld = { surfel.T, surfel.B, surfel.N };
	// Change normal according to bump map
	surfel.N = normalize(mul(BumpTex * 2 - 1, TangentToWorld));

	material.Diffuse *= DiffTex.xyz * (MaskTex.x); // set transparent if necessary.
	material.Specular.xyz = max(material.Specular.xyz, SpecularTex);
}

void GetIndices(out int transformIndex, out int materialIndex, out int triangleIndex, out int vertexOffset) {
	transformIndex = ObjectInfo.TransformIndex;
	materialIndex = ObjectInfo.MaterialIndex;
	triangleIndex = ObjectInfo.StartTriangle + PrimitiveIndex();
	vertexOffset = ObjectInfo.VertexOffset;
}

void GetHitInfo(
	float3 barycentrics, 
	int materialIndex,
	int triangleIndex, 
	int vertexOffset,
	int transformIndex,
	out Vertex surfel, 
	out Material material, 
	out VolumeMaterial volumeMaterial,
	float ddx, float ddy)
{
	Vertex v1 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 0] + vertexOffset];
	Vertex v2 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 1] + vertexOffset];
	Vertex v3 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 2] + vertexOffset];
	Vertex s = {
		v1.P * barycentrics.x + v2.P * barycentrics.y + v3.P * barycentrics.z,
		v1.N * barycentrics.x + v2.N * barycentrics.y + v3.N * barycentrics.z,
		v1.C * barycentrics.x + v2.C * barycentrics.y + v3.C * barycentrics.z,
		v1.T * barycentrics.x + v2.T * barycentrics.y + v3.T * barycentrics.z,
		v1.B * barycentrics.x + v2.B * barycentrics.y + v3.B * barycentrics.z
	};

	surfel = Transform(s, Transforms[transformIndex]);

	material = Materials[materialIndex];
	volumeMaterial = VolMaterials[materialIndex];
	AugmentHitInfoWithTextureMapping(surfel, material, ddx, ddy);
}
