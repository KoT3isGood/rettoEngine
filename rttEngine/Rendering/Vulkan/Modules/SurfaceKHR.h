#pragma once
#include "Module.h"
#include "Instance.h"
#include <Windows.h>
#include "Vulkan\vulkan_win32.h"
namespace rttvk {
	class SurfaceKHR: public Module {
	public:
		SurfaceKHR() = default;
		SurfaceKHR(Instance* instance, HWND window, HINSTANCE hInstance);
		virtual void Create() override;
		virtual void Destroy() override;
		VkSurfaceKHR GetSurface();
	private:
		HWND window;
		HINSTANCE hInstance;
		Instance* instance;
		VkSurfaceKHR surface;
	};
}