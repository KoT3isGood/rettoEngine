
#pragma once

#ifdef Win64
#include <windows.h>
#include <tchar.h>
#include "Utils\Surface\Surface.h"

#include "Utils\Layers\Layer.h"
namespace rttvk {

	class SurfaceKHR;
}
class VulkanLayer;
#include "Rendering\Vulkan\VulkanLayer.h"

/*
* Win32 Window Generator
* Converts 
*/
class Win64Surface: public Surface {
public:
	float previousTime = 0.0;
	float currentTime = 0.0;
	float timeToTitleUpdate = 0.0;

	// Default Constructor
	Win64Surface() = default;

	// Constructor
	Win64Surface(const char* windowName);

	// Destructor
	~Win64Surface();

	// ~Update surface
	virtual void StartUpdateLoop() override;

	// Handler of the window
	HWND Handle = nullptr;

	HINSTANCE instance = nullptr;
private:
	Layer renderingLayers;

	VulkanLayer* vkLayer = nullptr;
	// Update loop status
	bool shouldRun = true;
	// ~Update surface

	// Window size
	RECT winSize = { 0,0,1280,720 };

	// Window handler
	static LRESULT CALLBACK WindowEventHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};
#endif