#include <Windows.h>
#include "dx4xb.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"

#include "scenes.h"

#include "Techniques/Examples/ClearRTSampleTechnique.h"
#include "Techniques/Examples/DemoTechnique.h"
#include "Techniques/Examples/BasicSceneTechnique.h"

using namespace dx4xb;

#define USE_GUI

#ifdef USE_GUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Create the presenter object
	PresenterDescription pDesc;
	pDesc.hWnd = hwnd;
	gObj<Presenter> presenter = Presenter::Create(pDesc);
	InternalDXInfo dxObjects;
	presenter->GetInternalDXInfo(dxObjects); // Get internal DX objects for ImGui management.

#ifdef USE_GUI
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	CComPtr<ID3D12DescriptorHeap> guiDescriptors;
	// Create GUI SRV Descriptor Heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (dxObjects.device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&guiDescriptors)) != S_OK)
			return false;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(dxObjects.device, dxObjects.Buffers,
		dxObjects.RenderTargetFormat, guiDescriptors,
		guiDescriptors->GetCPUDescriptorHandleForHeapStart(),
		guiDescriptors->GetGPUDescriptorHandleForHeapStart());

#endif

	// Create the technique and load
	gObj<BasicSceneTechnique> technique = new BasicSceneTechnique();
	
	gObj<SceneManager> scene = new BunnyScene();
	scene->SetupScene();

	if (technique.Dynamic_Cast<IManageScene>())
		technique->SetSceneManager(scene);

	presenter->Load(technique);
	
	// Main graphics loop
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

#ifdef USE_GUI
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (true)
		{
			ImGui::Begin("Stats");                          
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
#endif
		
		presenter->Dispatch_Technique(technique);

#ifdef USE_GUI

		// Prepares the render target to draw on it...
		// Just in case the technique leave it in other state
		//presenter _dispatch RenderTarget();

		auto renderTargetHandle = dxObjects.RenderTargets[dxObjects.swapChain->GetCurrentBackBufferIndex()];
		dxObjects.mainCmdList->OMSetRenderTargets(1, &renderTargetHandle, false, nullptr);
		ID3D12DescriptorHeap* dh[1] = { guiDescriptors };
		dxObjects.mainCmdList->SetDescriptorHeaps(1, dh);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxObjects.mainCmdList);
#endif

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
