#include <Windows.h>
#include "dx4xb.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"

#include "scenes.h"

#include "Techniques/Examples/ClearRTSampleTechnique.h"
#include "Techniques/Examples/DemoTechnique.h"
#include "Techniques/Examples/BasicSceneTechnique.h"
#include "Techniques/Examples/DepthComplexityTechnique.h"
#include "Techniques/Examples/BasicRaycastSample.h"
#include "Techniques/Pathtracing/PathtracingTechnique.h"
#include "Techniques/Pathtracing/NEEPathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/CVAEPathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/NEECVAEPathtracingTechnique.h"
#include "Techniques/CVAEPathtracing/STFTechnique.h"
#include "Techniques/CVAEPathtracing/STFXTechnique.h"

using namespace dx4xb;

#define USE_GUI
#define SAVE_STATS

#ifdef USE_GUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma region Saving RenderTarget as image

class ScreenShotTechnique : public Technique {
public:
	gObj<Texture2D> TextureToSave;

	dx4xb::string FileName;

	void OnLoad() {
		TextureToSave = CreateTexture2DUAV<RGBA>(CurrentRenderTarget()->Width(), CurrentRenderTarget()->Height());
	}

	void CopyRenderTarget(gObj<GraphicsManager> manager) {
		manager->Copy(TextureToSave, CurrentRenderTarget());
		manager->FromGPU(TextureToSave);
	}

	virtual void OnDispatch() override {
		Execute_OnGPU(CopyRenderTarget);
		Flush().WaitFor();
		Save(TextureToSave, FileName);
	}
};

#pragma endregion

#pragma region Saving Image as Float Array

class ImageSavingTechnique : public Technique {
public:
	gObj<Texture2D> TextureToSave;

	dx4xb::string FileName;

	void OnLoad() {
	}

	void CopyImageFromGPU(gObj<GraphicsManager> manager) {
		manager->FromGPU(TextureToSave);
	}

	virtual void OnDispatch() override {
		Execute_OnGPU(CopyImageFromGPU);
		Flush().WaitFor();

		FILE* writting;
		if (fopen_s(&writting, FileName.c_str(), "wb") != 0)
			return;

		int dataSize = TextureToSave->Width() * TextureToSave->Height() * TextureToSave->ElementStride();
		byte* data = new byte[dataSize];
		TextureToSave->Read(data);
		fwrite((void*)data, 1, dataSize, writting);
		fclose(writting);
	}
};

#pragma endregion

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
	gObj<PathtracingTechnique> technique = new PathtracingTechnique();
	//gObj<NEEPathtracingTechnique> technique = new NEEPathtracingTechnique();
	//gObj<CVAEPathtracingTechnique> technique = new CVAEPathtracingTechnique();
	//gObj<NEECVAEPathtracingTechnique> technique = new NEECVAEPathtracingTechnique();
	//gObj<STFTechnique> technique = new STFTechnique();
	//gObj<STFXTechnique> technique = new STFXTechnique();
	
	gObj<ScreenShotTechnique> takingScreenshot;
	gObj<ImageSavingTechnique> savingStats;

	gObj<SceneManager> scene = new LucyAndDrago2();
	scene->SetupScene();

	if (technique.Dynamic_Cast<IManageScene>())
		technique->SetSceneManager(scene);

	presenter->Load(technique);

	presenter->Load(takingScreenshot);
	presenter->Load(savingStats);
	
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
		
		presenter->ExecuteTechnique(technique);

#ifdef SAVE_STATS
		gObj<IGatherImageStatistics> asStatistics = technique.Dynamic_Cast<IGatherImageStatistics>();
		if (asStatistics) {

			// Save Sum, SqrSum and frames
			gObj<Texture2D> sumTexture;
			gObj<Texture2D> sqrSumTexture;
			int frames;
			asStatistics->getAccumulators(sumTexture, sqrSumTexture, frames);

			if (frames > 1)
				if ((frames & (frames - 1)) == 0) // power of 2
				{
					char number[100];
					_itoa_s(frames, number, 10);
					dx4xb::string fileName = "save_";
					fileName = fileName + dx4xb::string(number);

					takingScreenshot->FileName = fileName + dx4xb::string(".png");
					presenter->ExecuteTechnique(takingScreenshot);

					savingStats->FileName = fileName + dx4xb::string("_sum.bin");
					savingStats->TextureToSave = sumTexture;
					presenter->ExecuteTechnique(savingStats); // saving sum

					savingStats->FileName = fileName + dx4xb::string("_sqrSum.bin");
					savingStats->TextureToSave = sqrSumTexture;
					presenter->ExecuteTechnique(savingStats); // saving sqr sum
				}
		}
#endif

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
