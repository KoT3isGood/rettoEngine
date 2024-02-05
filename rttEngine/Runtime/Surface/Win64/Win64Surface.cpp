
#ifdef Win64
#include "Win64Surface.h"

Win64Surface::Win64Surface(const char* windowName)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	// Create wc
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = L"rttWindow";
	wc.lpfnWndProc = WindowEventHandler;
	// If failed to create
	RTT_ASSERT(RegisterClassEx(&wc));
	
	

	// Convert char to w_chart
	const WCHAR* windowNameWide;
	int nChars = MultiByteToWideChar(CP_ACP, 0, windowName, -1, NULL, 0);
	windowNameWide = new WCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, windowName, -1, (LPWSTR)windowNameWide, nChars);

	// Create window properties
	DWORD windowProperties = WS_SYSMENU;
	
	// Set Resolution
	AdjustWindowRect(&winSize, windowProperties, false);

	// Create window
	Handle = CreateWindowEx(
		NULL,
		L"rttWindow", // ???
		windowNameWide,
		windowProperties,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winSize.right - winSize.left,
		winSize.bottom - winSize.top,
		NULL,
		NULL,
		NULL,
		NULL
	);

	RTT_ASSERT(Handle);


	SetWindowLongPtr(Handle, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(Handle, SW_SHOW);
	UpdateWindow(Handle);




	vkLayer = new VulkanLayer();
	renderingLayers.PushBack(vkLayer);
}

Win64Surface::~Win64Surface()
{
	delete vkLayer;
	// Gets run multiple times
	DestroyWindow(Handle);
}

void Win64Surface::StartUpdateLoop()
{
	// Loop
	MSG msg;
	while (shouldRun) {


		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		Sleep(1);
	}
}
LRESULT Win64Surface::WindowEventHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
	{
		Win64Surface* window = (Win64Surface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->shouldRun = false;

		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return NULL;
}

#endif