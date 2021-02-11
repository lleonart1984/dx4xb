#pragma once

#include "dx4xb_scene.h"
#include "gui_traits.h"

class DepthComplexityTechnique : public Technique, public IManageScene {
public:

	~DepthComplexityTechnique() {}

	gObj<Buffer> VertexBuffer;
	gObj<Buffer> IndexBuffer;
	gObj<Buffer> Camera;
	gObj<Buffer> GeometryTransforms;
	gObj<Buffer> InstanceTransforms;
	gObj<Texture2D> Complexity;

	struct CameraCB {
		float4x4 Projection;
		float4x4 View;
	};

	struct Pipeline : public GraphicsPipeline {

		gObj<Texture2D> CountingBuffer;
		gObj<Buffer> InstanceTransforms;
		gObj<Buffer> GeometryTransforms;
		gObj<Buffer> Camera;

		struct ObjectInfoCB {
			int InstanceIndex;
			int TransformIndex;
			int MaterialIndex;
		} ObjectInfo;

		// Inherited via GraphicsPipelineBindings
		void Setup() override {
			set->VertexShader(ShaderLoader::FromFile("./Techniques/Examples/Basic_VS.cso"));
			set->PixelShader(ShaderLoader::FromFile("./Techniques/Examples/Counting_PS.cso"));
			set->InputLayout(SceneVertex::Layout());
		}

		void Bindings(gObj<GraphicsBinder> binder) {
			binder->Bindings_OnSet(); {
				binder->Bindings_PixelShader();
				binder->UAV(0, CountingBuffer);

				binder->Bindings_VertexShader();
				binder->SRV(0, InstanceTransforms);
				binder->SRV(1, GeometryTransforms);
				binder->CBV(0, Camera);
			}

			binder->Bindings_OnDispatch(); {
				binder->Bindings_VertexShader();
				binder->CBV(1, ObjectInfo);
			}
		}
	};
	gObj<Pipeline> pipeline;
	gObj<ShowComplexityPipeline> showComplexity;
	gObj<Buffer> screenVertices;

	// Inherited via Technique
	virtual void OnLoad() override {

		auto desc = scene->getScene();

		// Allocate Memory for scene elements
		VertexBuffer = Create_Buffer_VB<SceneVertex>(desc->Vertices().Count);
		IndexBuffer = Create_Buffer_IB<int>(desc->Indices().Count);
		Camera = Create_Buffer_CB<CameraCB>();
		GeometryTransforms = Create_Buffer_SRV<float4x3>(desc->getTransformsBuffer().Count);
		InstanceTransforms = Create_Buffer_SRV<float4x4>(desc->Instances().Count);
		Complexity = Create_Texture2D_UAV<uint>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		Load(showComplexity);
		Load(pipeline);
		
		pipeline->Camera = Camera;
		pipeline->GeometryTransforms = GeometryTransforms;
		pipeline->InstanceTransforms = InstanceTransforms;
		pipeline->CountingBuffer = Create_Texture2D_DSV(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		screenVertices = Create_Buffer_VB<float2>(6);
		screenVertices->Write_List({
			float2(-1, -1), float2(1, -1), float2(1, 1),
			float2(-1, -1), float2(1, 1), float2(-1, 1)
			});

		Execute_OnGPU(UpdateDirtyElements);
		Execute_OnGPU(LoadScreenVertices);
	}

	void LoadScreenVertices(gObj<GraphicsManager> manager) {
		manager->Load_AllToGPU(screenVertices);
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
			Camera->Write_Value(CameraCB{
					proj,
					view
				});
			manager->Load_AllToGPU(Camera);
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

	virtual void OnDispatch() override {
		// Update dirty elements
		Execute_OnGPU(UpdateDirtyElements);

		// Draw current Frame
		Execute_OnGPU(DrawScene);
		Execute_OnGPU(DrawComplexity);
	}

	void DrawScene(gObj<GraphicsManager> manager) {
		pipeline->CountingBuffer = Complexity;

		manager->Set_Pipeline(pipeline);
		manager->Set_IndexBuffer(IndexBuffer);
		manager->Set_Viewport(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Clear_RT(CurrentRenderTarget(), float3(0.2f, 0.2f, 0.5f));
		manager->Clear_UAV(Complexity, uint4(0));

		auto desc = scene->getScene();

		for (int i = 0; i < desc->Instances().Count; i++) {
			pipeline->ObjectInfo.InstanceIndex = i;
			InstanceDescription instance = desc->Instances().Data[i];
			for (int j = 0; j < instance.Count; j++) {
				GeometryDescription geometry = desc->Geometries().Data[instance.GeometryIndices[j]];

				manager->Set_VertexBuffer(VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount));

				pipeline->ObjectInfo.TransformIndex = geometry.TransformIndex;
				pipeline->ObjectInfo.MaterialIndex = geometry.MaterialIndex;
				manager->Dispatch_IndexedTriangles(geometry.IndexCount, geometry.StartIndex);
			}
		}
	}

	void DrawComplexity(gObj<GraphicsManager> manager) {
		showComplexity->RenderTarget = CurrentRenderTarget();
		showComplexity->Complexity = Complexity;

		manager->Set_Pipeline(showComplexity);
		manager->Set_VertexBuffer(screenVertices);
		manager->Set_Viewport(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
		manager->Dispatch_Triangles(6);
	}
};