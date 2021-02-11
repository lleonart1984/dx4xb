#pragma once

#include "dx4xb_scene.h"
#include "gui_traits.h"

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
				binder->Bindings_OnSet();
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
		VertexBuffer = Create_Buffer_SRV<SceneVertex>(desc->Vertices().Count);
		IndexBuffer = Create_Buffer_SRV<int>(desc->Indices().Count);
		Transforms = Create_Buffer_CB<TransformsCB>();
		GeometryTransforms = Create_Buffer_SRV<float4x3>(desc->getTransformsBuffer().Count);
		InstanceTransforms = Create_Buffer_SRV<float4x4>(desc->Instances().Count);
		OutputImage = Create_Texture2D_UAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		Load(pipeline);
		pipeline->Transforms = Transforms;
		pipeline->Output = OutputImage;

		Execute_OnGPU(UpdateDirtyElements);

		Execute_OnGPU(CreateSceneOnGPU);
	}

	void UpdateDirtyElements(gObj<GraphicsManager> manager) {

		auto elements = scene->Updated(sceneVersion);
		auto desc = scene->getScene();

		if (+(elements & SceneElement::Vertices))
		{
			VertexBuffer->Write_Ptr(desc->Vertices().Data);
			manager->Load_AllToGPU(VertexBuffer);
		}

		if (+(elements & SceneElement::Indices))
		{
			IndexBuffer->Write_Ptr(desc->Indices().Data);
			manager->Load_AllToGPU(IndexBuffer);
		}

		if (+(elements & SceneElement::Camera))
		{
			float4x4 proj, view;
			scene->getCamera().GetMatrices(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height(), view, proj);
			Transforms->Write_Value(TransformsCB{
					mul(inverse(proj), inverse(view))
				});
			manager->Load_AllToGPU(Transforms);
		}

		if (+(elements & SceneElement::GeometryTransforms))
		{
			GeometryTransforms->Write_Ptr(desc->getTransformsBuffer().Data);
			manager->Load_AllToGPU(GeometryTransforms);
		}

		if (+(elements & SceneElement::InstanceTransforms))
		{
			float4x4* transforms = new float4x4[desc->Instances().Count];
			for (int i = 0; i < desc->Instances().Count; i++)
				transforms[i] = desc->Instances().Data[i].Transform;

			InstanceTransforms->Write_Ptr(transforms);
			manager->Load_AllToGPU(InstanceTransforms);
			delete[] transforms;
		}
	}

	gObj<InstanceCollection> rtxScene;

	void CreateSceneOnGPU(gObj<RaytracingManager> manager) {

		auto desc = scene->getScene();

		rtxScene = manager->Create_Intances();
		for (int i = 0; i < desc->Instances().Count; i++)
		{
			auto instance = desc->Instances().Data[i];

			auto geometryCollection = manager->Create_TriangleGeometries();
			geometryCollection->Set_Transforms(GeometryTransforms);

			for (int j = 0; j < instance.Count; j++) // load every geometry
			{
				auto geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

				if (IndexBuffer)
					geometryCollection->Create_Geometry(
						VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount),
						IndexBuffer->Slice(geometry.StartIndex, geometry.IndexCount),
						geometry.TransformIndex);
				else
					geometryCollection->Create_Geometry(
						VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount),
						geometry.TransformIndex);
			}

			manager->Load_Geometry(geometryCollection);

			rtxScene->Create_Instance(geometryCollection,
				255U, 0, i, (float4x3)instance.Transform
			);
		}

		manager->Load_Scene(rtxScene, true, true);

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

			rtxScene->Load_InstanceTransform(i, (float4x3)instance.Transform);
		}

		manager->Load_Scene(rtxScene, true, true);
	}

	void DrawScene(gObj<RaytracingManager> manager) {

		manager->Set_Pipeline(pipeline);
		manager->Set_Program(pipeline->MainProgram);

		static bool firstTime = true;
		if (firstTime) {
			manager->Set_RayGeneration(pipeline->Generating);
			manager->Set_Miss(pipeline->Missing, 0);
			manager->Set_HitGroup(pipeline->Hitting, 0);
			firstTime = false;
		}

		manager->Dispatch_Rays(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Copy_Resource(CurrentRenderTarget(), OutputImage);
	}
};