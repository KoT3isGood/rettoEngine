#pragma once
#include "Runtime\Layers\RenderingLayer.h"


#include "Vulkan\vulkan.h"

namespace rttvk{
}
// Win32
#ifdef Win64
#include <windows.h>
#include "Vulkan\vulkan_win32.h"

class Win64Surface;
#include "Runtime\Surface\Win64\Win64Surface.h"
#endif

#include <chrono>

#include <vector>
#include "Runtime\Application\ProcessInfo.h"



#include "Modules\Instance.h"
#include "Modules\DebugMessenger.h"
#include "Modules\LogicalDevice.h"
#include "Modules\SurfaceKHR.h"
#include "Modules\Swapchain.h"
#include "Modules\ImageView.h"
#include "Rendering\ShaderCompiler\Shader.h"
#include "Modules\Pipeline.h"
#include "Modules\CommandPool.h"
#include "Modules\CommandBuffer.h"
#include "Modules\SyncGPU.h"
#include "Modules\Buffer.h"
#include "Modules\RayTracing\RTPipeline.h"
#define VK_VALIDATE(result, vkStruct) if(result!=VK_SUCCESS) {RTT_LOG(std::string("[ VULKAN ] FAILED TO USE ")+#vkStruct);RTT_ASSERT(0);}

class VulkanLayer : public RenderingLayer {
public:
	VulkanLayer();
	~VulkanLayer();
	virtual void Draw() override;

	float resolution[2];
private:
	rttvk::Instance instance = rttvk::Instance({
		VK_KHR_SURFACE_EXTENSION_NAME,
		// TODO: ADD IFDEF FOR DIFFERENT PLATFORMS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,

		VK_EXT_DEBUG_UTILS_EXTENSION_NAME

		},{
			"VK_LAYER_KHRONOS_validation",
			//"VK_LAYER_LUNARG_api_dump"
		});

	rttvk::DebugMessenger debugMessenger = rttvk::DebugMessenger(&instance);

	VkPhysicalDevice chosenPhysicalDevice;
	rttvk::LogicalDevice logicalDevice = rttvk::LogicalDevice(&chosenPhysicalDevice, {
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME,
			"VK_KHR_buffer_device_address"
		},{

		});
	// TODO: ADD IFDEF FOR DIFFERENT PLATFORMS
	Win64Surface* win64surface = (Win64Surface*)getProcessInfo()->surface;

	rttvk::SurfaceKHR surface = rttvk::SurfaceKHR();
	rttvk::Swapchain swapchain = rttvk::Swapchain(&surface, &logicalDevice);
	std::vector<VkImage> images;
	std::vector<rttvk::ImageView> imageViews;

	rttvk::Shader computeShader = rttvk::Shader();
	rttvk::Pipeline pipeline = rttvk::Pipeline(&computeShader,&logicalDevice);

	rttvk::CommandPool commandPool = rttvk::CommandPool(&logicalDevice);
	rttvk::CommandBuffer commandBuffer = rttvk::CommandBuffer(&logicalDevice, &commandPool);

	rttvk::Semaphore renderFinished = rttvk::Semaphore(&logicalDevice);
	rttvk::Semaphore imageAvailable = rttvk::Semaphore(&logicalDevice);
	rttvk::Fence inFlightFence = rttvk::Fence(&logicalDevice);

	VkDescriptorPool descPool;
	VkDescriptorSet descSet;


	rttvk::Buffer buffer = rttvk::Buffer(&logicalDevice,8,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	rttvk::Shader rayGenShader = rttvk::Shader();
	std::vector<rttvk::Shader*> rayTracingShaders = {&rayGenShader };
	VkDescriptorSetLayoutBinding setLayout1{0,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1};
	std::vector<VkDescriptorSetLayoutBinding> rtSetLayout = { setLayout1 };












	VkImage renderingImage;
	VkDeviceMemory memory;
	void* mapped;
	rttvk::ImageView renderingImageView = rttvk::ImageView();
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(chosenPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}



	void RecordCommandBuffer(uint32_t imageIndex);
	void ChangeImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
private:

	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;

	VkDescriptorPool descPoolRT;
	VkDescriptorSet descSetRT;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties = {};

	rttvk::Buffer rayGenBuffer = rttvk::Buffer(&logicalDevice, 64, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR);

	VkStridedDeviceAddressRegionKHR rgenShaderBindingTable = {};

	uint32_t align(uint32_t a, uint32_t x) {
		return uint32_t((a + (uint32_t(x) - 1)) & ~uint32_t(x - 1));
	}

	rttvk::RTPipeline rtPipeline = rttvk::RTPipeline(&logicalDevice, rayTracingShaders, rtSetLayout);
};