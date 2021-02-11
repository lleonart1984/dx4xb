#pragma once

#include "dx4xb.h"

using namespace dx4xb;

struct TriangleSample : public Technique {
	gObj<Buffer> vertexBuffer;
	gObj<Buffer> indexBuffer;

	struct Vertex {
		float3 P;
	};

	struct Pipeline : public GraphicsPipeline {
		gObj<Texture2D> RenderTarget;
		gObj<Texture2D> Texture;

		// Inherited via GraphicsPipeline
		void Setup() override {
			set->VertexShader(ShaderLoader::FromFile("./Techniques/Examples/Demo_VS.cso"));
			set->PixelShader(ShaderLoader::FromFile("./Techniques/Examples/Demo_PS.cso"));
			set->InputLayout({
					VertexElement { VertexElementType::Float, 3, "POSITION" }
				});
		}

		void Bindings(gObj<GraphicsBinder> binder) {
			binder->Bindings_OnSet();
			{
				binder->Bindings_PixelShader();
				binder->RTV(0, RenderTarget);
				binder->SRV(0, Texture);
				binder->SMP_Static(0, Sampler::Linear());
			}
		}
	};
	gObj<Pipeline> pipeline;
	gObj<Texture2D> texture;

	// Inherited via Technique
	void OnLoad() override {
		vertexBuffer = Create_Buffer_VB<Vertex>(4);
		vertexBuffer->Write_List({
			Vertex { float3(-1, -1, 0.5) },
			Vertex { float3(1, -1, 0.5) },
			Vertex { float3(1, 1, 0.5) },
			Vertex { float3(-1, 1, 0.5) }
			});

		indexBuffer = Create_Buffer_IB<int>(6);
		indexBuffer->Write_List({
			 0, 1, 2, 0, 2, 3
			});

		texture = Create_Texture2D_SRV<float4>(2, 2, 2, 1);
		float4 pixels[] = {
				float4(1,0,0,1), float4(1,1,0,1),
				float4(0,1,0,1), float4(0,0,1,1),
				float4(1,0,1,1)
		};
		texture->Write_Ptr(pixels);

		float4 pixel = float4(1, 0, 1, 1);
		texture->Write_Element(0, 0, pixel);

		Load(pipeline);

		Execute_OnGPU(LoadAssets);
	}

	void LoadAssets(gObj<CopyManager> manager) {
		manager->Load_AllToGPU(vertexBuffer);
		manager->Load_AllToGPU(indexBuffer);
		manager->Load_AllToGPU(texture);
	}

	void OnDispatch() override {
		Execute_OnGPU(DrawTriangle);
	}

	void DrawTriangle(gObj<GraphicsManager> manager) {
		static int frame = 0;
		frame++;
		pipeline->RenderTarget = CurrentRenderTarget();
		pipeline->Texture = texture;
		manager->Set_Pipeline(pipeline);
		manager->Set_VertexBuffer(vertexBuffer);
		manager->Set_IndexBuffer(indexBuffer->Slice(3, 3));
		manager->Set_Viewport(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());

		manager->Clear_RT(CurrentRenderTarget(), float3(0.2f, sin(frame * 0.001), 0.5f));

		manager->Dispatch_IndexedTriangles(3);
	}

};
