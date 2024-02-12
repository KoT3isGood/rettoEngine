#include "SurfaceKHR.h"
namespace rttvk {
	SurfaceKHR::SurfaceKHR(Instance* instance, HWND window, HINSTANCE hInstance)
	{
		this->window = window;
		this->hInstance = hInstance;
		this->instance = instance;
	}
	void SurfaceKHR::Create()
	{
		// TODO: IMPLEMENT IFDEF TO CHECK IF PLATFORM IS WIDNOW, LINUX OR ELSE;
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = window;
		createInfo.hinstance = hInstance;

		VK_CREATE_VALIDATION(vkCreateWin32SurfaceKHR(instance->GetInstance(),&createInfo,nullptr,&surface), VkSurfaceKHR);
	}
	void SurfaceKHR::Destroy()
	{
		vkDestroySurfaceKHR(instance->GetInstance(), surface, nullptr);
	}
	VkSurfaceKHR SurfaceKHR::GetSurface()
	{
		return surface;
	}
}