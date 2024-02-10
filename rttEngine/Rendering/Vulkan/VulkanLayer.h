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

#include "Rendering\ShaderCompiler\Shader.h"

#include <vector>
#include "Runtime\Application\ProcessInfo.h"



#include "Modules\Instance.h"
#include "Modules\DebugMessenger.h"
#include "Modules\LogicalDevice.h"

class VulkanLayer : public RenderingLayer {
public:
	VulkanLayer();
	~VulkanLayer();
	virtual void Draw() override;
private:
	rttvk::Instance instance = rttvk::Instance({
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		},{
			"VK_LAYER_KHRONOS_validation"
		});
	rttvk::DebugMessenger debugMessenger = rttvk::DebugMessenger(&instance);
	VkPhysicalDevice chosenPhysicalDevice;
	rttvk::LogicalDevice logicalDevice = rttvk::LogicalDevice(&chosenPhysicalDevice, {
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		},{

		});
};