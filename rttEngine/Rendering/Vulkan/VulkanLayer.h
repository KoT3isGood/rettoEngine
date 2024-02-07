#pragma once
#include "Runtime\Layers\RenderingLayer.h"


#include "Vulkan\vulkan.h"


// Win32
#ifdef Win64
#include <windows.h>
#include "Vulkan\vulkan_win32.h"

class Win64Surface;
#include "Runtime\Surface\Win64\Win64Surface.h"
#endif

#include <vector>
#include "Runtime\Application\ProcessInfo.h"


class VulkanLayer : public RenderingLayer {
public:
	VulkanLayer();
	~VulkanLayer();
	virtual void Draw() override;
private:
	bool validationLayers = false;


	//~Instance
	VkInstance instance;

	void CreateInstance();
	//~Instance

	//~Validation Layers
	VkDebugUtilsMessengerEXT messenger;

	void CreateValidationLayers();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_types,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data
	);
	//~Validation Layers

	// ~Physical Device
	VkPhysicalDevice physicalDevice;

	void SelectPhysicalDevice();
	// ~Physical Device

	// ~Logical Device
	VkDevice logicalDevice;

	void CreateLogicalDevice();
	// ~Logical Device

	// ~Queues
	void GetQueues();

	uint32_t graphicsFamily = -1;
	uint32_t presentFamily = -1;
	// ~Queues

	// ~Surface
	VkSurfaceKHR surface;

	void CreateSurface();
	// ~Surface

	// ~Swapchain
	VkSwapchainKHR swapchain;
	void CreateSwapchain();
	// ~Swapchain

	// ~Images

	void CreateImageViews();

	std::vector<VkImage> swImages;
	std::vector<VkImageView> swImageViews;
	// ~Images
};