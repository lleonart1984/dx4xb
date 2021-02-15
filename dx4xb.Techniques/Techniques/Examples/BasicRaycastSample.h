#pragma once

#include "dx4xb_scene.h"
#include "../../gui_traits.h"

using namespace dx4xb;

class BasicRaycastSample : public Technique, public IManageScene {
public:

	~BasicRaycastSample() {}

	struct TransformsCB {
		float4x4 FromProjectionToWorld;
	};

	gObj<Buffer> VertexBuffer;
	gObj<Buffer> IndexBuffer;
	gObj<Buffer> Transforms;
	gObj<Buffer> GeometryTransforms;
	gObj<Buffer> InstanceTransforms;
	gObj<Texture2D> OutputImage;

	struct RTXSample : public RaytracingPipeline {
		struct Program : public RaytracingProgram<RTXSample> {

			void Setup() {
				Shader(Context()->Generating);
				Shader(Context()->Missing);
				Shader(Context()->Hitting);
			}

			void Bindings(gObj<RaytracingBinder> binder) {
				binder->OnSet();
				binder->ADS(0, Context()->Scene);
				binder->UAV(0, Context()->Output);
				binder->CBV(0, Context()->Transforms);
			}
		};
		gObj<Program> MainProgram;

		void Setup() {
			Code(ShaderLoader::FromFile("./Techniques/Examples/RTXSample_RT.cso"));
			Shader(Generating, L"RayGen");
			Shader(Missing, L"OnMiss");
			gObj<ClosestHitHandle> closestHit;
			Shader(closestHit, L"OnClosestHit");
			HitGroup(Hitting, closestHit, nullptr, nullptr);
			RTProgram(MainProgram);
		}

		gObj<RayGenerationHandle> Generating;
		gObj<HitGroupHandle> Hitting;
		gObj<MissHandle> Missing;

		gObj<InstanceCollection> Scene;
		gObj<Texture2D> Output;
		gObj<Buffer> Transforms;
	};
	gObj<RTXSample> pipeline;

	// Inherited via Technique
	virtual void OnLoad() override {

		auto desc = scene->getScene();

		// Allocate Memory for scene elements
		VertexBuffer = CreateBufferSRV<SceneVertex>(desc->Vertices().Count);
		IndexBuffer = CreateBufferSRV<int>(desc->Indices().Count);
		Transforms = CreateBufferCB<TransformsCB>();
		GeometryTransforms = CreateBufferSRV<float4x3>(desc->getTransformsBuffer().Count);
		InstanceTransforms = CreateBufferSRV<float4x4>(desc->Instances().Count);
		OutputImage = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		Load(pipeline);
		pipeline->Transforms = Transforms;
		pipeline->Output = OutputImage;

		Execute_OnGPU(UpdateDirtyElements);

		Execute_OnGPU(CreateSceneOnGPU);
	}

	void UpdateDirtyElements(gObj<RaytracingManager> manager) {

		auto elements = scene->Updated(sceneVersion);
		auto desc = scene->getScene();

		if (+(elements & SceneElement::Vertices))
		{
			VertexBuffer->Write(desc->Vertices().Data);
			RaytracingManager* m;
			((GraphicsManager*)m)->ToGPU(gObj<Buffer>());
			manager->ToGPU(VertexBuffer);
		}

		if (+(elements & SceneElement::Indices))
		{
			IndexBuffer->Write(desc->Indices().Data);
			manager->ToGPU(IndexBuffer);
		}

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			Transforms->Write(TransformsCB{
					mul(inverse(proj), inverse(view))
				});
			manager->ToGPU(Transforms);
		}

		if (+(elements & SceneElement::GeometryTransforms))
		{
			GeometryTransforms->Write(desc->getTransformsBuffer().Data);
			manager->ToGPU(GeometryTransforms);
		}

		if (+(elements & SceneElement::InstanceTransforms))
		{
			float4x4* transforms = new float4x4[desc->Instances().Count];
			for (int i = 0; i < desc->Instances().Count; i++)
				transforms[i] = desc->Instances().Data[i].Transform;

			InstanceTransforms->Write(transforms);
			manager->ToGPU(InstanceTransforms);
			delete[] transforms;
		}
	}

	gObj<InstanceCollection> rtxScene;

	void CreateSceneOnGPU(gObj<RaytracingManager> manager) {

		auto desc = scene->getScene();

		rtxScene = manager->CreateIntances();
		for (int i = 0; i < desc->Instances().Count; i++)
		{
			auto instance = desc->Instances().Data[i];

			auto geometryCollection = manager->CreateTriangleGeometries();
			geometryCollection->UseTransforms(GeometryTransforms);

			for (int j = 0; j < instance.Count; j++) // load every geometry
			{
				auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

				if (IndexBuffer)
					geometryCollection->CreateGeometry(
						VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount),
						IndexBuffer->Slice(geometry.StartIndex, geometry.IndexCount),
						geometry.TransformIndex);
				else
					geometryCollection->CreateGeometry(
						VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount),
						geometry.TransformIndex);
			}

			manager->ToGPU(geometryCollection, false, true);

			rtxScene->CreateInstance(geometryCollection,
				255U, 0, i, (float4x3)instance.Transform
			);
		}

		manager->ToGPU(rtxScene, true, true);

		pipeline->Scene = rtxScene;
	}

	virtual void OnDispatch() override {
		// Update dirty elements
		Execute_OnGPU(UpdateDirtyElements);

		Execute_OnGPU(UpdateSceneOnGPU);

		// Draw current Frame
		Execute_OnGPU(DrawScene);
	}

	void UpdateSceneOnGPU(gObj<RaytracingManager> manager) {
		auto desc = scene->getScene();

		for (int i = 0; i < desc->Instances().Count; i++)
		{
			auto instance = desc->Instances().Data[i];

			rtxScene->UpdateTransform(i, (float4x3)instance.Transform);
		}

		manager->ToGPU(rtxScene, true, true);
	}

	void DrawScene(gObj<RaytracingManager> manager) {

		manager->SetPipeline(pipeline);
		manager->SetProgram(pipeline->MainProgram);

		static bool firstTime = true;
		if (firstTime) {
			manager->SetRayGeneration(pipeline->Generating);
			manager->SetMiss(pipeline->Missing, 0);
			manager->SetHitGroup(pipeline->Hitting, 0);
			firstTime = false;
		}

		manager->DispatchRays(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Copy(CurrentRenderTarget(), OutputImage);
	}
};