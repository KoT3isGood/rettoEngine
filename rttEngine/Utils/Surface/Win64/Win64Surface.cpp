
#ifdef Win64
#include "Win64Surface.h"

Win64Surface::Win64Surface(const char* windowName)
{
	getProcessInfo()->surface = this;

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
	DWORD windowProperties = WS_OVERLAPPEDWINDOW;
	
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
	resolutionX = winSize.right - winSize.left;
	resolutionY = winSize.bottom - winSize.top;
	
	instance = GetModuleHandle(NULL);


	vkLayer = new VulkanLayer();
	getProcessInfo()->renderingLayer = vkLayer;
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
	MSG msg;
	while (shouldRun) {
		rttGUI()->NewFrame();

		currentTime = getRunningTime();
		float delta = currentTime - previousTime;
		previousTime = currentTime;
		
		GetCurrentLevel()->Tick(delta);

		timeToTitleUpdate += delta;
		/*if (timeToTitleUpdate>=1.0f ) {
			timeToTitleUpdate = 0;
			float fps = 1 / delta;
			std::string tmp = std::to_string(fps);
			if (fps > 10000) {
				tmp = "this NASA PC enjoyer has " + std::to_string(fps) + " fps";
			}

			SetWindowTextA(Handle, tmp.c_str());
		}*/

		RECT rMyRect;
		GetWindowRect(Handle, (LPRECT)&rMyRect);

		positionX = rMyRect.top;
		positionY = rMyRect.left;

		vkLayer->resolution[0] = resolutionX;
		vkLayer->resolution[1] = resolutionY;

		// Loop
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);;
			DispatchMessageW(&msg);
		}
		vkLayer->Draw();

		rttGUI()->Render();
		
	}
}
LRESULT Win64Surface::WindowEventHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win64Surface* window = (Win64Surface*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (uMsg) {
	case WM_DESTROY:
	{
		
		window->shouldRun = false;

		break;
	}
	case WM_SIZE:
		window->resolutionX = LOWORD(lParam);
		window->resolutionY = HIWORD(lParam);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return NULL;
}

#endif