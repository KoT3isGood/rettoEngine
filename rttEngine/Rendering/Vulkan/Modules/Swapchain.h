#pragma once
#include "Module.h"
#include "SurfaceKHR.h"
#include "LogicalDevice.h"

namespace rttvk {
	class Swapchain : public Module {
	public:
		Swapchain(SurfaceKHR* surface, LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		VkSwapchainKHR GetSwapchain();
		bool isOk = true;
	private:
		VkSwapchainKHR swapchain;
		LogicalDevice* device;
		SurfaceKHR* surface;
	};
}