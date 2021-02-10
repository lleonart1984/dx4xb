#include <Windows.h>
#include "dx4xb.h"

using namespace dx4xb;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct MyTechnique : public Technique {
	void OnLoad() {
	}

	void ClearRenderTarget(gObj<GraphicsManager> manager) {
		manager->Clear_RT(CurrentRenderTarget(), float4(1, 1, 0, 1));
	}

	void OnDispatch() {
		Execute_OnGPU(ClearRenderTarget);
	}
};

struct TriangleSample : public Technique {
	gObj<Buffer> vertexBuffer;
	gObj<Buffer> indexBuffer;

	struct Vertex {
		float3 P;
	};

	struct Pipeline : public GraphicsPipeline {

		gObj<Texture2D> RenderTarget;
		gObj<Texture2D> Texture;

		// Inherited via GraphicsPipelineBindings
		void Setup() override {
			set->VertexShader(ShaderLoader::FromFile("./Shaders/Samples/Demo_VS.cso"));
			set->PixelShader(ShaderLoader::FromFile("./Shaders/Samples/Demo_PS.cso"));
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
		texture->Write_Ptr((byte*)pixels);

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

// Main code
int main(int, char**)
{
	int ClientWidth = 1264;
	int ClientHeight = 761;

	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("CA4G Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, TEXT("DirectX12 Example with dx4xb"), WS_OVERLAPPEDWINDOW, 100, 100,
		ClientWidth + (1280 - 1264), ClientHeight + (800 - 761),
		NULL, NULL, wc.hInstance, NULL);

	// Create the presenter object
	PresenterDescription pDesc;
	pDesc.hWnd = hwnd;
	gObj<Presenter> presenter = Presenter::Create(pDesc);

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	gObj<TriangleSample> technique;
	presenter->Load(technique);
	
	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		presenter->BeginFrame();
		
		presenter->Dispatch_Technique(technique);

		presenter->EndFrame();
	}

	return 0;
}

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#ifdef USE_GUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
#endif
	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
