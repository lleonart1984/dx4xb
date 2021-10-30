
/// Common header to any library that will use the scene retained as follow.
/// Provides functions to get intersection information
/// Augment material and surfel with texture mapping (including bump mapping)

#include "Definitions.h"

StructuredBuffer<Vertex> VertexBuffer			: register(t0, space1);	// All vertices in scene.
StructuredBuffer<int> IndexBuffer				: register(t1, space1); // Indices of geometries in scene.
StructuredBuffer<float4x3> Transforms			: register(t2, space1); // All materials.
StructuredBuffer<Material> Materials			: register(t3, space1); // All materials.
StructuredBuffer<VolumeMaterial> VolMaterials	: register(t4, space1); // All materials.
Texture3D<float> Grids[10]						: register(t5, space1); // All Grids can be used in this pathtracer
Texture2D<float4> Textures[500]					: register(t15, space1); // All textures used

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
SamplerState VolumeSampler : register(s1, space1);

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
	//surfel.N = normalize(surfel.N);// normalize(mul(float3(0.5, 0.5, 1) * 2 - 1, TangentToWorld));

	material.Diffuse *= DiffTex.xyz;// *(MaskTex.x); // set transparent if necessary.
	material.Specular.xyz = max(material.Specular.xyz, SpecularTex);
	material.RefractionIndex *= MaskTex.x;
}

void GetIndices(out int transformIndex, out int materialIndex, out int triangleIndex, out int vertexOffset) {
	transformIndex = ObjectInfo.TransformIndex;
	materialIndex = ObjectInfo.MaterialIndex;
	triangleIndex = ObjectInfo.StartTriangle + PrimitiveIndex();
	vertexOffset = ObjectInfo.VertexOffset;
}

bool GetHitInfo(
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
	if (vertexOffset >= 0) // surface hit 
	{
		float3 coords = float3(1 - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);

		Vertex v1 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 0] + vertexOffset];
		Vertex v2 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 1] + vertexOffset];
		Vertex v3 = VertexBuffer[IndexBuffer[triangleIndex * 3 + 2] + vertexOffset];
		Vertex s = {
			v1.P * coords.x + v2.P * coords.y + v3.P * coords.z,
			v1.N * coords.x + v2.N * coords.y + v3.N * coords.z,
			v1.C * coords.x + v2.C * coords.y + v3.C * coords.z,
			v1.T * coords.x + v2.T * coords.y + v3.T * coords.z,
			v1.B * coords.x + v2.B * coords.y + v3.B * coords.z
		};

		surfel = Transform(s, Transforms[transformIndex]);
	}
	else
	{
		surfel = (Vertex)0;
		surfel.P = mul(float4(barycentrics, 1), Transforms[transformIndex]).xyz;
	}
	material = Materials[materialIndex];
	volumeMaterial = VolMaterials[materialIndex];
	AugmentHitInfoWithTextureMapping(surfel, material, ddx, ddy);

	return vertexOffset >= 0;
}

