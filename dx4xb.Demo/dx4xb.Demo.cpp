#include <Windows.h>
#include "dx4xb.h"

using namespace dx4xb;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct MyTechnique : public Technique {
	void OnLoad() {
	}

	void OnDispatch() {
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

	gObj<MyTechnique> technique;
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
		
		presenter->Dispatch(technique);

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
