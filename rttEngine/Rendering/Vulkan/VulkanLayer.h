#pragma once
#include "Runtime\Layers\RenderingLayer.h"
#include "Vulkan\vulkan.h"
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
};