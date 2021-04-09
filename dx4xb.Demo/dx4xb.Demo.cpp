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
#include "Techniques/VolumeRendering/VPTTechnique.h"

using namespace dx4xb;

#define USE_GUI
//#define SAVE_STATS
//#define OFFLINE

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
		delete[] data;
	}
};

#pragma endregion

#pragma region GUI support in ImGUI

//void GuiFor(gObj<IShowComplexity> t) {
//	ImGui::Checkbox("View Complexity", &t->ShowComplexity);
//	ImGui::SliderFloat("Pathtracing", &t->PathtracingRatio, 0, 1);
//}

int selectedMaterial = 0;

void GuiFor(gObj<IManageScene> t) {

#pragma region camera setup

	Camera camera = t->scene->getCamera();
	if (
		ImGui::InputFloat3("View position", (float*)&camera.Position) |
		ImGui::InputFloat3("Target position", (float*)&camera.Target))
		t->scene->setCamera(camera);

#pragma endregion

#pragma region lighting setup

	LightSource l = t->scene->getMainLight();

	float alpha = dx4xb::fmod(atan2f(l.Direction.z, l.Direction.x) + 2 * PI, 2 * PI);
	float beta = asinf(l.Direction.y);
	bool changeDirection = ImGui::SliderFloat("Light Direction Alpha", &alpha, 0, 3.141596 * 2);
	changeDirection |= ImGui::SliderFloat("Light Direction Beta", &beta, -3.141596 / 2, 3.141596 / 2);
	if (changeDirection |
		ImGui::InputFloat3("Light intensity", (float*)&l.Intensity)) {
		l.Direction = float3(cosf(alpha) * cosf(beta), sinf(beta), sinf(alpha) * cosf(beta));
		t->scene->setMainLightSource(l);
	}

#pragma endregion

	ImGui::SliderInt("Material Index", &selectedMaterial, -1, t->scene->getScene()->Materials().Count);

#pragma region Modifying Material

	if (selectedMaterial >= 0)
	{
		bool materialModified = false;

		materialModified |= ImGui::SliderFloat3(
			"Diffuse",
			(float*)&t->scene->getScene()->Materials().Data[selectedMaterial].Diffuse,
			0.0f,
			1.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic
		);

		materialModified |= ImGui::SliderFloat3(
			"Specular",
			(float*)&t->scene->getScene()->Materials().Data[selectedMaterial].Specular,
			0.0f,
			1.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic
		);

		materialModified |= ImGui::SliderFloat3(
			"Emissive",
			(float*)&t->scene->getScene()->Materials().Data[selectedMaterial].Emissive,
			0.0f,
			1000.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic
		);

		materialModified |= ImGui::SliderFloat(
			"Refraction Index",
			(float*)&t->scene->getScene()->Materials().Data[selectedMaterial].RefractionIndex,
			0.0f,
			1.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic
		);

		if (ImGui::SliderFloat4(
			"Models",
			(float*)&t->scene->getScene()->Materials().Data[selectedMaterial].Roulette,
			0.0f,
			1.0f,
			"%.3f",
			ImGuiSliderFlags_Logarithmic
		)) {
			// Normalize roulette values
			auto r = t->scene->getScene()->Materials().Data[selectedMaterial].Roulette;
			r = r / maxf(0.001, r.x + r.y + r.z + r.w);
			t->scene->getScene()->Materials().Data[selectedMaterial].Roulette = r;
			materialModified = true;
		}

		VolumeMaterial& vol = t->scene->getScene()->VolumeMaterials().Data[selectedMaterial];

		float3 extinction = vol.Extinction;
		float size = max(0.001, max(extinction.x, max(extinction.y, extinction.z)));
		extinction = maxf(0.0001, extinction * 1.0 / size);
		float3 absorption = 1 - vol.ScatteringAlbedo;

		if (
			ImGui::SliderFloat("Size", (float*)&size, 0.01, 1000, "%.3f", ImGuiSliderFlags_Logarithmic) |
			ImGui::SliderFloat3("Extinction", (float*)&extinction, 0.0, 1.0) |
			ImGui::SliderFloat3("Absorption", (float*)&absorption, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic) |
			ImGui::SliderFloat3("G", (float*)&vol.G, -0.999, 0.999)
			) {
			vol.Extinction = extinction * size;
			vol.ScatteringAlbedo = 1 - absorption;
			materialModified = true;
		}


		if (materialModified)
			t->scene->MakeDirty(SceneElement::Materials);
	}

#pragma endregion
}

template<typename T>
void RenderGUI(gObj<Technique> t) {
	gObj<T> h = t.Dynamic_Cast<T>();
	if (h)
		GuiFor(h);
}

#pragma endregion

// Main code
int main(int, char**)
{
	int ClientWidth = 1264;
	//int ClientWidth = 512;
	int ClientHeight = 761;
	//int ClientHeight = 512;

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
#ifdef OFFLINE
	pDesc.Frames = 1;
#else
	pDesc.hWnd = hwnd; // Comment to have an offscreen renderer
	pDesc.Frames = 2;
#endif
	pDesc.ResolutionWidth = ClientWidth;
	pDesc.ResolutionHeight = ClientHeight;
	pDesc.UseBuffering = false;
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
	gObj<VPTTechnique> technique = new VPTTechnique();
	//gObj<BasicSceneTechnique> technique = new BasicSceneTechnique();
	//gObj<BasicRaycastSample> technique = new BasicRaycastSample();
	//gObj<PathtracingTechnique> technique = new PathtracingTechnique();
	//gObj<NEEPathtracingTechnique> technique = new NEEPathtracingTechnique();
	//gObj<CVAEPathtracingTechnique> technique = new CVAEPathtracingTechnique();
	//gObj<NEECVAEPathtracingTechnique> technique = new NEECVAEPathtracingTechnique();
	//gObj<STFTechnique> technique = new STFTechnique();
	//gObj<STFXTechnique> technique = new STFXTechnique();
	
	gObj<ScreenShotTechnique> takingScreenshot;
	gObj<ImageSavingTechnique> savingStats;
	
	//gObj<SceneManager> scene = new BuddhaScene();
	//gObj<SceneManager> scene = new LucyAndDrago3();
	gObj<SceneManager> scene = new CloudScene();
	//gObj<SceneManager> scene = new BunnySceneForPT();
	//gObj<SceneManager> scene = new BunnyScene();
	//gObj<SceneManager> scene = new BunnyCornellScene();
	//gObj<SceneManager> scene = new Sponza();
	scene->SetupScene();

	if (technique.Dynamic_Cast<IManageScene>())
		technique.Dynamic_Cast<IManageScene>()->SetSceneManager(scene);

	presenter->Load(technique);

	presenter->Load(takingScreenshot);
	presenter->Load(savingStats);

	int animationFrame = 0;
	int totalAnimatedFrames = 40;
	
	//scene->Animate(animationFrame / (float)totalAnimatedFrames, animationFrame);
	char timeNumber[100];
	
	long startTickcount = GetCurrentTime();
	_itoa_s(GetCurrentTime() - startTickcount, timeNumber, 10);
	std::cout << timeNumber << std::endl;

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

			RenderGUI<IManageScene>(technique);

			ImGui::End();

			auto camera = scene->getCamera();

			bool cameraChanged = false;

			auto delta = ImGui::GetMouseDragDelta(1);

			if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space)))
				camera.RotateAround(delta.x * 0.01f, -delta.y * 0.01f);
			else
				camera.Rotate(delta.x * 0.01f, -delta.y * 0.01f);

			auto deltaTime = ImGui::GetIO().DeltaTime;

			if (delta.x != 0 || delta.y != 0)
			{
				cameraChanged = true;
				ImGui::ResetMouseDragDelta(1);
			}
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
			{
				camera.MoveForward(deltaTime);
				cameraChanged = true;
			}
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
			{
				camera.MoveBackward(deltaTime);
				cameraChanged = true;
			}
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
			{
				camera.MoveLeft(deltaTime);
				cameraChanged = true;
			}
			if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
			{
				camera.MoveRight(deltaTime);
				cameraChanged = true;
			}

			if (cameraChanged)
				scene->setCamera(camera);
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

			//if (frames >= (1 << 11))
			if (frames >= (1 << 4))
				if ((frames & (frames - 1)) == 0) // power of 2
				{
					long span = GetCurrentTime() - startTickcount;
					_itoa_s(span, timeNumber, 10);
					std::cout << timeNumber << std::endl;

					std::cout << "Left: " << (span * (totalAnimatedFrames - animationFrame - 1) / (animationFrame + 1)) << std::endl;

					char number[100];
					_itoa_s(frames, number, 10);
					//_itoa_s(animationFrame, number, 10);
					dx4xb::string fileName = "./Teaser/save_";
					fileName = fileName + dx4xb::string(number);

					takingScreenshot->FileName = fileName + dx4xb::string(".png");
					presenter->ExecuteTechnique(takingScreenshot);

					savingStats->FileName = fileName + dx4xb::string("_sum.bin");
					savingStats->TextureToSave = sumTexture;
					presenter->ExecuteTechnique(savingStats); // saving sum

					savingStats->FileName = fileName + dx4xb::string("_sqrSum.bin");
					savingStats->TextureToSave = sqrSumTexture;
					presenter->ExecuteTechnique(savingStats); // saving sqr sum
					
					//animationFrame++;
					//scene->Animate(animationFrame / (float)totalAnimatedFrames, animationFrame);
					//std::cout << fileName.c_str() << std::endl;

					//if (animationFrame >= totalAnimatedFrames)
					//{
					//	break; // finish animations
					//}
				}
		}
#endif

#ifdef USE_GUI

		// Prepares the render target to draw on it...
		// Just in case the technique leave it in other state
		//presenter _dispatch RenderTarget();

		auto renderTargetHandle = dxObjects.RenderTargets[dxObjects.swapChain == nullptr ? 0 : dxObjects.swapChain->GetCurrentBackBufferIndex()];
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
