#pragma once
#include "Utils\Layers\RenderingLayer.h"


#include "Vulkan\vulkan.h"

namespace rttvk{
}
// Win32
#ifdef Win64
#include <windows.h>
#include "Vulkan\vulkan_win32.h"

class Win64Surface;
#include "Utils\Surface\Win64\Win64Surface.h"
#endif

#include <chrono>

#include <vector>
struct ProcessInfo;
#include "Utils\Application\ProcessInfo.h"



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
#include "Modules\RayTracing\BLAS.h"
#include "Modules\RayTracing\TLAS.h"
#include "Modules\Texture.h"
#include "Modules\Image.h"
#include "Utils\VeryCoolMath\Math.h"


#include "Utils\MeshLoader\MeshData.h"
#include "Utils\MeshLoader\Obj/ObjLoader.h"
#define VK_VALIDATE(result, vkStruct) if(result==-4) {\
RTT_LOG(std::string("HOW DID YOU LOSE YOUR GPU? ")); \
}

class VulkanLayer : public RenderingLayer {
public:
	VulkanLayer();
	~VulkanLayer();
	virtual void Draw() override;

	float resolution[2];
	float usablePosition[2];
private:
	rttvk::Instance instance = rttvk::Instance({
		VK_KHR_SURFACE_EXTENSION_NAME,
		// TODO: ADD IFDEF FOR DIFFERENT PLATFORMS
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,

		VK_EXT_DEBUG_UTILS_EXTENSION_NAME

		},{
			//"VK_LAYER_LUNARG_api_dump",
			//"VK_LAYER_KHRONOS_validation"
		});

	rttvk::DebugMessenger debugMessenger = rttvk::DebugMessenger(&instance);

	VkPhysicalDevice chosenPhysicalDevice;
	rttvk::LogicalDevice logicalDevice = rttvk::LogicalDevice(&chosenPhysicalDevice, {
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME,
			VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME
		},{

		});
	// TODO: ADD IFDEF FOR DIFFERENT PLATFORMS
	Win64Surface* win64surface = (Win64Surface*)getProcessInfo()->surface;

	rttvk::SurfaceKHR surface = rttvk::SurfaceKHR();
	rttvk::Swapchain swapchain = rttvk::Swapchain(&surface, &logicalDevice);
	std::vector<VkImage> images;
	std::vector<rttvk::ImageView> imageViews;

	void Resize();

	VkDescriptorSetLayoutBinding albedoInputRT{ 9,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding directInputRT{ 10,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding indirectInputRT{ 11,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding normalInputRT{ 12,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding worldInputRT{ 13,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding direct2InputRT{ 14,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding indirect2InputRT{ 15,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };

	rttvk::Shader computeShader = rttvk::Shader();
	VkDescriptorSetLayoutBinding imageInput{ 0,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding resBinding{ 1,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding hierarchyCountBinding{ 2,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding hierarchyBinding{ 3,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,1,VK_SHADER_STAGE_COMPUTE_BIT };
	VkDescriptorSetLayoutBinding fontBinding{ 4,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,VK_SHADER_STAGE_COMPUTE_BIT };
	std::vector<VkDescriptorSetLayoutBinding> setLayout = { imageInput, resBinding,hierarchyCountBinding,hierarchyBinding,fontBinding, albedoInputRT,directInputRT,indirectInputRT };
	rttvk::Pipeline pipeline = rttvk::Pipeline(&computeShader,&logicalDevice, setLayout);

	rttvk::Shader atrousShader = rttvk::Shader();
	std::vector<VkDescriptorSetLayoutBinding> setLayoutAtrous = {directInputRT,indirectInputRT,normalInputRT,worldInputRT,direct2InputRT,indirect2InputRT };
	rttvk::Pipeline atrousPipeline = rttvk::Pipeline(&atrousShader, &logicalDevice, setLayoutAtrous,4);
	VkDescriptorPool descPoolAtrous;
	VkDescriptorSet descSetAtrous;

	rttvk::CommandPool commandPool = rttvk::CommandPool(&logicalDevice);
	rttvk::CommandBuffer commandBuffer = rttvk::CommandBuffer(&logicalDevice, &commandPool);

	rttvk::Semaphore renderFinished = rttvk::Semaphore(&logicalDevice);
	rttvk::Semaphore imageAvailable = rttvk::Semaphore(&logicalDevice);
	rttvk::Fence inFlightFence = rttvk::Fence(&logicalDevice);

	VkDescriptorPool descPool;
	VkDescriptorSet descSet;


	rttvk::Buffer buffer = rttvk::Buffer(&logicalDevice,8,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	rttvk::Buffer bufferHierarchyAmount = rttvk::Buffer(&logicalDevice, 4, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	rttvk::Buffer cameraPositionBuffer = rttvk::Buffer(&logicalDevice, 48, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	float pos[3][4] = {-5,0,0,0};










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

	rttvk::Buffer resolutionBuffer;
private:

	// Ray tracing

	rttvk::Shader rayGenShader = rttvk::Shader();
	rttvk::Shader missShader = rttvk::Shader();
	rttvk::Shader closestHitShader = rttvk::Shader();
	rttvk::Shader shadowMissShader = rttvk::Shader();
	std::vector<rttvk::Shader*> rayTracingShaders = { &rayGenShader, &missShader, &shadowMissShader, &closestHitShader};
	VkDescriptorSetLayoutBinding imageInputRT {0,VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1 };
	VkDescriptorSetLayoutBinding tlasBinding{ 1,VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,1};
	VkDescriptorSetLayoutBinding resBindingRT{ 2,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1 };
	VkDescriptorSetLayoutBinding noiseBinding{ 3,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1 };
	VkDescriptorSetLayoutBinding cameraPosBindingRT{ 4,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1 };
	VkDescriptorSetLayoutBinding lightsCountBindingRT{ 5,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1 };
	VkDescriptorSetLayoutBinding lightsBindingRT{ 6,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,1 };
	VkDescriptorSetLayoutBinding meshesBindingRT{ 7,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,1 };
	VkDescriptorSetLayoutBinding texturesBindingRT{ 8,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,64 };
	std::vector<VkDescriptorSetLayoutBinding> rtSetLayout = { imageInputRT, tlasBinding, resBindingRT, noiseBinding, cameraPosBindingRT,lightsCountBindingRT,lightsBindingRT,meshesBindingRT,texturesBindingRT,albedoInputRT,directInputRT,indirectInputRT,normalInputRT,worldInputRT };

	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;

	VkDescriptorPool descPoolRT;
	VkDescriptorSet descSetRT;

	VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProperties = {};

	rttvk::Buffer shaderBindingTable;

	VkStridedDeviceAddressRegionKHR rgenRegion = {};
	VkStridedDeviceAddressRegionKHR rmissRegion = {};
	VkStridedDeviceAddressRegionKHR rchitRegion = {};

	uint32_t align(uint32_t value, uint32_t alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	rttvk::RTPipeline rtPipeline = rttvk::RTPipeline(&logicalDevice, rayTracingShaders, rtSetLayout);

	bool hasbuilt = false;

	std::vector <rttvk::BLAS> blases = {};
	rttvk::TLAS tlas = rttvk::TLAS(&logicalDevice,&meshes, &blases);
	rttvk::Texture blueNoise = rttvk::Texture(&logicalDevice,"Content/Textures/bluenoise256.png");

	rttvk::Buffer lightsCountBuffer = rttvk::Buffer(&logicalDevice,4,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	rttvk::Buffer lightsBuffer = rttvk::Buffer();
	std::vector <LightSourceShader> lightsShader = {};
	std::vector <rttvk::Texture> textures;
	std::vector<VkDescriptorImageInfo> imageInfos;
private:
	// rttGUI
	rttvk::Texture font = rttvk::Texture(&logicalDevice, "Content/Textures/rttguifont.png");
	rttvk::Buffer rttGUIDrawHierarchy = rttvk::Buffer();

	// Rendering
	rttvk::Image albedo = rttvk::Image(&logicalDevice,1280,720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image direct = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image indirect = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image normal = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image worldPos = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image direct2 = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
	rttvk::Image indirect2 = rttvk::Image(&logicalDevice, 1280, 720, VK_FORMAT_R32G32B32_SFLOAT);
};