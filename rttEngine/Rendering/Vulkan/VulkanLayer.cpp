#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{
	
	instance.Create();
	debugMessenger.Create();

	// Selecting Physical Device
	// TODO: IMPLEMENT BETTER SYSTEM
	uint32_t pdCount = 0;
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, nullptr);
	RTT_ASSERT(pdCount);
	std::vector<VkPhysicalDevice> physicalDevices(pdCount);
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, physicalDevices.data());
	chosenPhysicalDevice = physicalDevices[0];

	logicalDevice.Create();

	surface = rttvk::SurfaceKHR(&instance, win64surface->Handle, win64surface->instance);
	surface.Create();
	swapchain.Create();

	// Get Image Count
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, nullptr);
	images.resize(imageCount);
	imageViews.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, images.data());

	for (int i = 0; i < imageViews.size();i++) {
		imageViews[i] = rttvk::ImageView(&logicalDevice, images[0]);
		imageViews[i].Create();
	}

	rttvk::Shader computeShader = rttvk::Shader("Content/EngineLoad/Shaders/shader.comp", &logicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);
	computeShader.Create();

	VkPipelineLayout layout;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	//pipelineLayoutInfo.setLayoutCount = 1;
	//pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;
	VK_CREATE_VALIDATION(vkCreatePipelineLayout(logicalDevice.GetDevice(), &pipelineLayoutInfo, nullptr, &layout), VkPipelineLayout);



	VkPipelineCache pc{};
	VkPipelineCacheCreateInfo cache{};
	cache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	VK_CREATE_VALIDATION(vkCreatePipelineCache(logicalDevice.GetDevice(), &cache,nullptr, &pc), VkPipelineCache);

	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.layout = layout;
	createInfo.stage = computeShader.shaderStageInfo;

	VkPipeline pipeline;
	
	VK_CREATE_VALIDATION(vkCreateComputePipelines(logicalDevice.GetDevice(), pc, 1, &createInfo, nullptr, &pipeline), VkPipeline);
	computeShader.Destroy();









	
}

VulkanLayer::~VulkanLayer()
{
	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i].Destroy();
	}
	swapchain.Destroy();
	surface.Destroy();
	logicalDevice.Destroy();
	debugMessenger.Destroy();
	instance.Destroy();
}

void VulkanLayer::Draw()
{
}
