#pragma once

#include "dx4xb_scene.h"
#include "../../gui_traits.h"
#include "../Tools/Parameters.h"

using namespace dx4xb;

class PathtracingTechniqueBase : public Technique, public IManageScene, public IGatherImageStatistics {

public:
	virtual ~PathtracingTechniqueBase() {}

	struct RTXPathtracingPipelineBase : public RaytracingPipeline {

		struct ProgramBase : public RaytracingProgram<RTXPathtracingPipelineBase> {

			void Setup() {
				Payload(28);
				Attribute(12); // 3-floats for barycentrics in 3D
				StackSize(1);

				Shader(Context()->Generating);
				Shader(Context()->Missing);
				Shader(Context()->Hitting);
				Shader(Context()->HittingWithMask);
				Shader(Context()->HittingVolume);
			}

			void Bindings(gObj<RaytracingBinder> binder) {
				binder->Space(2); // Random space
				binder->UAV(0, Context()->RngStates);
				
				binder->Space(1);
				binder->UAV(0, Context()->OutputImage);
				binder->UAV(1, Context()->Accumulation);
				binder->UAV(2, Context()->SqrAccumulation);
				binder->UAV(3, Context()->Complexity);
				binder->SRV(0, Context()->VertexBuffer);
				binder->SRV(1, Context()->IndexBuffer);
				binder->SRV(2, Context()->Transforms);
				binder->SRV(3, Context()->Materials);
				binder->SRV(4, Context()->VolMaterials);
				binder->SRV_Array(5, Context()->Grids, Context()->GridCount, 10);
				binder->SRV_Array(15, Context()->Textures, Context()->TextureCount);
				binder->SMP_Static(0, Sampler::Linear());
				binder->SMP_Static(1, Sampler::LinearWithoutMipMaps());
				binder->CBV(1, Context()->AccumulativeInfo);

				binder->Space(0);

				binder->ADS(0, Context()->Scene);
				binder->CBV(0, Context()->Lighting);
				binder->CBV(1, Context()->ProjectionToWorld);
			}

			void HitGroup_Bindings(gObj<RaytracingBinder> binder) {
				binder->Space(1);
				binder->CBV(0, Context()->PerGeometry);
			}
		};
		
		gObj<ProgramBase> MainProgram;

		virtual void CreateProgram(gObj<ProgramBase>& program) {
			program = new ProgramBase();
		}

		virtual const char* ShaderFile() {
			throw Exception::FromError(Errors::ShaderNotFound, "You must override this method to return a valid shader file.");
		}

		void Setup() {
			Code(ShaderLoader::FromFile(ShaderFile()));
			Shader(Generating, L"RayGen");
			Shader(Missing, L"OnMiss");
			gObj<ClosestHitHandle> closestHit;
			Shader(closestHit, L"OnClosestHit");
			gObj<AnyHitHandle> checkMaskOnHit;
			Shader(checkMaskOnHit, L"CheckMaskOnHit");
			gObj<IntersectionHandle> volumeIntersection;
			Shader(volumeIntersection, L"DeltaTracking");
			//gObj<AnyHitHandle> volumeAnyHit;
			//Shader(volumeAnyHit, L"DeltaTrackingAnyHit");
			HitGroup(HittingWithMask, closestHit, checkMaskOnHit, nullptr);
			HitGroup(Hitting, closestHit, nullptr, nullptr);
			HitGroup(HittingVolume, closestHit, nullptr, volumeIntersection);
			CreateProgram(MainProgram);
			RTProgram(MainProgram);
		}

		gObj<RayGenerationHandle> Generating;
		gObj<HitGroupHandle> Hitting;
		gObj<HitGroupHandle> HittingWithMask;
		gObj<HitGroupHandle> HittingVolume;
		gObj<MissHandle> Missing;

		// Space 1 (CommonRT.h, CommonPT.h)
		gObj<Buffer> VertexBuffer;
		gObj<Buffer> IndexBuffer;
		gObj<Buffer> Transforms;
		gObj<Buffer> Materials;
		gObj<Buffer> VolMaterials;
		gObj<Texture3D>* Grids;
		int GridCount;
		gObj<Texture2D>* Textures;
		int TextureCount;
		gObj<Texture2D> OutputImage;
		gObj<Texture2D> Accumulation;
		gObj<Texture2D> SqrAccumulation;
		gObj<Texture2D> Complexity;

		// uint4 texture needed for random states per ray.
		gObj<Texture2D> RngStates;

		struct PerGeometryInfo {
			int StartTriangle;
			int VertexOffset;
			int TransformIndex;
			int MaterialIndex;
		} PerGeometry;

		struct AccumulativeInfoCB {
			int Pass;
			int ShowComplexity;
			float PathtracingRatio;
		} AccumulativeInfo;

		// Space 0
		gObj<InstanceCollection> Scene;
		gObj<Buffer> Lighting;
		gObj<Buffer> ProjectionToWorld;
	};
	gObj<RTXPathtracingPipelineBase> pipeline;

	struct LightingCB {
		float3 LightPosition;
		float rem0;
		float3 LightIntensity;
		float rem1;
		float3 LightDirection;
		float rem2;
	};

	// Used to build bottom level ADS
	gObj<Buffer> GeometryTransforms;
	gObj<Buffer> VolumeGeometries;

	void getAccumulators(gObj<Texture2D>& sum, gObj<Texture2D>& sqrSum, int& frames)
	{
		sum = pipeline->Accumulation;
		sqrSum = pipeline->SqrAccumulation;
		frames = pipeline->AccumulativeInfo.Pass;
	}

	virtual void CreatePipeline(gObj<RTXPathtracingPipelineBase>& pipeline)
	{
		throw Exception::FromError(Errors::UnsupportedFormat, "You must override this method to create a valid pipeline");
	}

	// Inherited via Technique
	virtual void OnLoad() override {

		auto desc = scene->getScene();

		CreatePipeline(pipeline); // Creates the speciallized pipeline
		Load(pipeline); // loads the pipeline.

		GeometryTransforms = CreateBufferSRV<float4x3>(desc->getTransformsBuffer().Count);

		int globalGeometryCount = 0;
		for (int i = 0; i < desc->Instances().Count; i++)
			globalGeometryCount += desc->Instances().Data[i].Count;
		globalGeometryCount += desc->Volumes().Count; // one global transform per volume

		// Allocate Memory for scene elements
		pipeline->VertexBuffer = CreateBufferSRV<SceneVertex>(desc->Vertices().Count);
		pipeline->IndexBuffer = CreateBufferSRV<int>(desc->Indices().Count);
		pipeline->Transforms = CreateBufferSRV<float4x3>(globalGeometryCount);
		pipeline->Materials = CreateBufferSRV<SceneMaterial>(desc->Materials().Count);
		pipeline->VolMaterials = CreateBufferSRV<VolumeMaterial>(desc->Materials().Count);
		pipeline->TextureCount = desc->getTextures().Count;
		pipeline->Textures = new gObj<Texture2D>[desc->getTextures().Count];
		for (int i = 0; i < pipeline->TextureCount; i++)
			pipeline->Textures[i] = LoadTexture2D(desc->getTextures().Data[i]);
		pipeline->GridCount = desc->getGrids().Count;
		pipeline->Grids = new gObj<Texture3D>[desc->getGrids().Count];
		VolumeGeometries = CreateBufferADS<D3D12_RAYTRACING_AABB>(pipeline->GridCount);
		for (int i = 0; i < pipeline->GridCount; i++)
		{
			pipeline->Grids[i] = LoadTexture3D(desc->getGrids().Data[i]);
			int width = pipeline->Grids[i]->Width();
			int height = pipeline->Grids[i]->Height();
			int depth = pipeline->Grids[i]->Depth();
			int maxDim = max(width, max(height, depth));
			float3 corner = float3(width, height, depth)*0.5/maxDim;
			VolumeGeometries->WriteElement(i, D3D12_RAYTRACING_AABB{
					-corner.x, -corner.y, -corner.z,
					corner.x, corner.y, corner.z
				});
		}
		pipeline->OutputImage = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		pipeline->Accumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		pipeline->SqrAccumulation = CreateTexture2DUAV<float4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), 1, 1);
		pipeline->Complexity = CreateTexture2DUAV<uint>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		
		pipeline->RngStates = CreateTexture2DUAV<uint4>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		pipeline->Lighting = CreateBufferCB<LightingCB>();
		pipeline->ProjectionToWorld = CreateBufferCB<float4x4>();
		pipeline->AccumulativeInfo = {};

#ifdef SHOW_COMPLEXITY
		pipeline->AccumulativeInfo.ShowComplexity = 1;
		pipeline->AccumulativeInfo.PathtracingRatio = 0.5;
#else
		pipeline->AccumulativeInfo.ShowComplexity = 0;
		pipeline->AccumulativeInfo.PathtracingRatio = 0.0;
#endif

		Execute_OnGPU(LoadAssets);

		Execute_OnGPU(CreateRTXScene);
	}

	void LoadAssets(gObj<GraphicsManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
	}

	virtual void OnDispatch() override {
		// Update dirty elements
		Execute_OnGPU(UpdateAssets);
		// Draw current Frame
		Execute_OnGPU(DrawScene);
	}

	void UpdateAssets(gObj<RaytracingManager> manager) {
		SceneElement elements = scene->Updated(sceneVersion);
		UpdateBuffers(manager, elements);
		if (+(elements & SceneElement::InstanceTransforms))
			UpdateRTXScene(manager);
		if (elements != SceneElement::None)
			pipeline->AccumulativeInfo.Pass = 0; // restart frame for Pathtracing...
	}

	virtual void UpdateBuffers(gObj<GraphicsManager> manager, SceneElement elements) {
		auto desc = scene->getScene();

		if (+(elements & SceneElement::Vertices))
		{
			pipeline->VertexBuffer->Write(desc->Vertices().Data);
			manager->ToGPU(pipeline->VertexBuffer);
		}

		if (+(elements & SceneElement::Indices))
		{
			pipeline->IndexBuffer->Write(desc->Indices().Data);
			manager->ToGPU(pipeline->IndexBuffer);
		}

		if (+(elements & SceneElement::Materials))
		{
			pipeline->Materials->Write(desc->Materials().Data);
			pipeline->VolMaterials->Write(desc->VolumeMaterials().Data);
			manager->ToGPU(pipeline->Materials);
			manager->ToGPU(pipeline->VolMaterials);
		}

		if (+(elements & SceneElement::Textures)) {
			for (int i = 0; i < pipeline->TextureCount; i++)
				manager->ToGPU(pipeline->Textures[i]);

			for (int i = 0; i < pipeline->GridCount; i++)
				manager->ToGPU(pipeline->Grids[i]);

			if (pipeline->GridCount > 0)
				manager->ToGPU(VolumeGeometries);
		}

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			pipeline->ProjectionToWorld->Write(mul(inverse(proj), inverse(view)));
			manager->ToGPU(pipeline->ProjectionToWorld);
		}

		if (+(elements & SceneElement::Lights))
		{
			pipeline->Lighting->Write(LightingCB{
					scene->getMainLight().Position, 0,
					scene->getMainLight().Intensity, 0,
					scene->getMainLight().Direction, 0
				});
			manager->ToGPU(pipeline->Lighting);
		}

		if (+(elements & SceneElement::GeometryTransforms))
		{
			GeometryTransforms->Write(desc->getTransformsBuffer().Data);
			manager->ToGPU(GeometryTransforms);
		}

		if (+(elements & SceneElement::GeometryTransforms) ||
			+(elements & SceneElement::InstanceTransforms))
		{
			int transformIndex = 0;
			for (int i = 0; i < desc->Instances().Count; i++)
			{
				auto instance = desc->Instances().Data[i];
				for (int j = 0; j < instance.Count; j++) {
					auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

					float4x4 geometryTransform = geometry.TransformIndex == -1 ?
						float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) :
						Transforms::FromAffine(desc->getTransformsBuffer().Data[geometry.TransformIndex]);

					pipeline->Transforms->WriteElement(transformIndex,
						(float4x3)mul(geometryTransform, instance.Transform)
					);

					transformIndex++;
				}
			}

			for (int i = 0; i < desc->Volumes().Count; i++)
			{
				pipeline->Transforms->WriteElement(transformIndex, (float4x3)desc->Volumes().Data[i].Transform);
				transformIndex++;
			}

			manager->ToGPU(pipeline->Transforms);
		}
	}

	gObj<InstanceCollection> rtxScene;

	void CreateRTXScene(gObj<RaytracingManager> manager) {

		auto desc = scene->getScene();

		rtxScene = manager->CreateIntances();
		int geometryOffset = 0;
		for (int i = 0; i < desc->Instances().Count; i++)
		{
			auto instance = desc->Instances().Data[i];

			auto geometryCollection = manager->CreateTriangleGeometries();
			//geometryCollection _set Transforms(GeometryTransforms);

			for (int j = 0; j < instance.Count; j++) // load every geometry
			{
				auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

				geometryCollection->CreateGeometry(
					pipeline->VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount),
					pipeline->IndexBuffer->Slice(geometry.StartIndex, geometry.IndexCount),
					-1);
			}

			manager->ToGPU(geometryCollection, false, true);

			rtxScene->CreateInstance(geometryCollection,
				255U, geometryOffset, i, (float4x3)instance.Transform
			);

			geometryOffset += instance.Count;
		}

		for (int i = 0; i < desc->Volumes().Count; i++)
		{
			auto volume = desc->Volumes().Data[i];

			auto geometryCollection = manager->CreateProceduralGeometries();

			geometryCollection->CreateGeometry(VolumeGeometries->Slice(volume.GridIndex, 1));
			
			manager->ToGPU(geometryCollection, false, true);

			rtxScene->CreateInstance(geometryCollection,
				255U, geometryOffset, i, (float4x3)volume.Transform
			);

			geometryOffset += 1;
		}

		manager->ToGPU(rtxScene, false, true);

		pipeline->Scene = rtxScene;
	}

	void UpdateRTXScene(gObj<RaytracingManager> manager) {

		auto desc = scene->getScene();

		for (int i = 0; i < desc->Instances().Count; i++)
		{
			auto instance = desc->Instances().Data[i];

			rtxScene->UpdateTransform(i, (float4x3)instance.Transform);
		}

		for (int i = 0; i < desc->Volumes().Count; i++)
		{
			auto volume = desc->Volumes().Data[i];

			rtxScene->UpdateTransform(i + desc->Instances().Count, (float4x3)volume.Transform);
		}

		manager->ToGPU(rtxScene, false, true);
	}

	void DrawScene(gObj<RaytracingManager> manager) {

		manager->SetPipeline(pipeline);
		manager->SetProgram(pipeline->MainProgram);

		static bool firstTime = true;
		if (firstTime) {
			manager->SetRayGeneration(pipeline->Generating);
			manager->SetMiss(pipeline->Missing, 0);

			auto desc = scene->getScene();

			int geometryOffset = 0;

			for (int i = 0; i < desc->Instances().Count; i++)
			{
				auto instance = desc->Instances().Data[i];
				for (int j = 0; j < instance.Count; j++) {
					auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];
					pipeline->PerGeometry.StartTriangle = geometry.StartIndex / 3;
					pipeline->PerGeometry.VertexOffset = geometry.StartVertex;
					pipeline->PerGeometry.MaterialIndex = geometry.MaterialIndex;
					pipeline->PerGeometry.TransformIndex = geometryOffset + j;

					if (geometry.MaterialIndex >= 0 && desc->Materials().Data[geometry.MaterialIndex].MaskMap >= 0)
						manager->SetHitGroup(pipeline->HittingWithMask, j, 0, 1, geometryOffset);
					else
						manager->SetHitGroup(pipeline->Hitting, j, 0, 1, geometryOffset);
				}
				geometryOffset += instance.Count;
			}

			for (int i = 0; i < desc->Volumes().Count; i++)
			{
				auto volume = desc->Volumes().Data[i];
				pipeline->PerGeometry.StartTriangle = volume.GridIndex;
				pipeline->PerGeometry.VertexOffset = -1; // indicating is a volume
				pipeline->PerGeometry.MaterialIndex = volume.MaterialIndex;
				pipeline->PerGeometry.TransformIndex = geometryOffset;
				manager->SetHitGroup(pipeline->HittingVolume, 0, 0, 1, geometryOffset);
				geometryOffset++;
			}

			firstTime = false;
		}

		if (pipeline->AccumulativeInfo.Pass == 0) // first frame after scene dirty
		{
			//pipeline->AccumulativeInfo.PathtracingRatio = 1;
			pipeline->AccumulativeInfo.Pass = 0;
			manager->ClearUAV(pipeline->Accumulation, uint4(0));
			manager->ClearUAV(pipeline->Complexity, uint4(0));
		}

		manager->DispatchRays(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Copy(CurrentRenderTarget(), pipeline->OutputImage);

		pipeline->AccumulativeInfo.Pass++;
	}
};