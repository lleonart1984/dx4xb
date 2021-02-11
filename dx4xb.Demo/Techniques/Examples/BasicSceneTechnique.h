#pragma once

#include "dx4xb_scene.h"
#include "gui_traits.h"

using namespace dx4xb;

class BasicSceneTechnique : public Technique, public IManageScene {
public:

	~BasicSceneTechnique() {}

	gObj<Buffer> VertexBuffer;
	gObj<Buffer> IndexBuffer;
	gObj<Buffer> Camera;
	gObj<Buffer> Lighting;
	gObj<Buffer> GeometryTransforms;
	gObj<Buffer> InstanceTransforms;
	gObj<Texture2D>* Textures;
	int TextureCount;

	struct CameraCB {
		float4x4 Projection;
		float4x4 View;
	};

	struct LightingCB {
		float3 Position;
		float3 Intensity;
	};

	struct Pipeline : public GraphicsPipeline {

		gObj<Texture2D> RenderTarget;
		gObj<Texture2D> DepthBuffer;
		gObj<Buffer> InstanceTransforms;
		gObj<Buffer> GeometryTransforms;
		gObj<Buffer> Camera;
		gObj<Texture2D> Texture;

		struct ObjectInfoCB {
			int InstanceIndex;
			int TransformIndex;
			int MaterialIndex;
		} ObjectInfo;

		// Inherited via GraphicsPipelineBindings
		void Setup() override {
			set->VertexShader(ShaderLoader::FromFile("./Techniques/Examples/Basic_VS.cso"));
			set->PixelShader(ShaderLoader::FromFile("./Techniques/Examples/Basic_PS.cso"));
			set->InputLayout(SceneVertex::Layout());
			set->DepthTest();
		}

		void Bindings(gObj<GraphicsBinder> binder) {
			binder->Bindings_OnSet();
			{
				binder->Bindings_PixelShader();
				binder->SMP_Static(0, Sampler::Linear());
				binder->RTV(0, RenderTarget);
				binder->DSV(DepthBuffer);
					  
				binder->Bindings_VertexShader();
				binder->SRV(0, InstanceTransforms);
				binder->SRV(1, GeometryTransforms);
				binder->CBV(0, Camera);
			}

			binder->Bindings_OnDispatch();
			{
				binder->Bindings_VertexShader();
				binder->CBV(1, ObjectInfo);
					  
				binder->Bindings_PixelShader();
				binder->SRV(0, Texture);
			}
		}
	};
	gObj<Pipeline> pipeline;

	// Inherited via Technique
	virtual void OnLoad() override {

		auto desc = scene->getScene();

		// Allocate Memory for scene elements
		VertexBuffer = Create_Buffer_VB<SceneVertex>(desc->Vertices().Count);
		IndexBuffer = Create_Buffer_IB<int>(desc->Indices().Count);
		Camera = Create_Buffer_CB<CameraCB>();
		Lighting = Create_Buffer_CB<LightingCB>();
		GeometryTransforms = Create_Buffer_SRV<float4x3>(desc->getTransformsBuffer().Count);
		InstanceTransforms = Create_Buffer_SRV<float4x4>(desc->Instances().Count);
		Textures = new gObj<Texture2D>[desc->getTextures().Count];
		TextureCount = desc->getTextures().Count;
		for (int i = 0; i < TextureCount; i++)
			Textures[i] = Create_Texture2D_SRV(desc->getTextures().Data[i]);
		
		Load(pipeline);
		pipeline->Camera = Camera;
		pipeline->GeometryTransforms = GeometryTransforms;
		pipeline->InstanceTransforms = InstanceTransforms;
		pipeline->DepthBuffer = Create_Texture2D_DSV(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		Execute_OnGPU(UpdateDirtyElements);
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

		if (+(elements & SceneElement::Lights))
		{
			Lighting->Write_Value(LightingCB{
					scene->getMainLight().Position,
					scene->getMainLight().Intensity
				});
			manager->Load_AllToGPU(Lighting);
		}

		if (+(elements & SceneElement::GeometryTransforms))
		{
			GeometryTransforms->Write_Ptr(desc->getTransformsBuffer().Data);
			manager->Load_AllToGPU(GeometryTransforms);
		}

		if (+(elements & SceneElement::Textures)) {
			for (int i = 0; i < TextureCount; i++)
				manager->Load_AllToGPU(Textures[i]);
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
	}

	void DrawScene(gObj<GraphicsManager> manager) {
		pipeline->RenderTarget = CurrentRenderTarget();
		manager->Set_Pipeline(pipeline);
		manager->Set_IndexBuffer(IndexBuffer);
		manager->Set_Viewport(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Clear_RT(CurrentRenderTarget(), float3(0.2f, 0.2f, 0.5f));
		manager->Clear_Depth(pipeline->DepthBuffer);

		auto desc = scene->getScene();

		for (int i = 0; i < desc->Instances().Count; i++) {
			pipeline->ObjectInfo.InstanceIndex = i;
			InstanceDescription instance = desc->Instances().Data[i];
			for (int j = 0; j < instance.Count; j++) {
				GeometryDescription geometry = desc->Geometries().Data[instance.GeometryIndices[j]];
				pipeline->ObjectInfo.TransformIndex = geometry.TransformIndex;
				pipeline->ObjectInfo.MaterialIndex = geometry.MaterialIndex;

				manager->Set_VertexBuffer(VertexBuffer->Slice(geometry.StartVertex, geometry.VertexCount));

				pipeline->Texture = nullptr;
				if (geometry.MaterialIndex != -1)
				{
					int textureIndex = desc->Materials().Data[geometry.MaterialIndex].DiffuseMap;
					if (textureIndex != -1)
						pipeline->Texture = Textures[textureIndex];
				}
				manager->Dispatch_IndexedTriangles(geometry.IndexCount, geometry.StartIndex);
			}
		}
	}
};

