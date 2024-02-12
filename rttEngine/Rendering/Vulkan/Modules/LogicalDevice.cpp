#include "LogicalDevice.h"

namespace rttvk {
	LogicalDevice::LogicalDevice(VkPhysicalDevice* physicalDevice, std::vector<const char*> extensions, std::vector<const char*> layers)
	{
		this->extensions = extensions;
		this->layers = layers;
		this->physicalDevice = physicalDevice;
	}
	void LogicalDevice::Create()
	{
		// Queues
		uint32_t queueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueCount, nullptr);
		std::vector<VkQueueFamilyProperties> queues(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueCount, queues.data());

		for (int i = 0; i < queues.size(); i++) {
			if (queues[i].queueFlags == VK_QUEUE_COMPUTE_BIT) {
				graphicsFamily = i;
				isGraphicsFamilyDone = true;
				presentFamily = i;
				isPresentFamilyDone = true;
			}
			if (isGraphicsFamilyDone && isPresentFamilyDone) {
				break;
			};
		}

		// TODO: Test if extension is enabled and then create features
		VkPhysicalDeviceAccelerationStructureFeaturesKHR accStructureFeatures{};
		accStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accStructureFeatures.accelerationStructure = VK_TRUE;

		VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};
		rtPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		rtPipelineFeatures.rayTracingPipeline = VK_TRUE;
		rtPipelineFeatures.pNext = &accStructureFeatures;

		// Device Create info
		float priority = 1.0; // Should Always be 1.0
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &priority;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = layers.size();
		createInfo.ppEnabledLayerNames = layers.data();
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.pNext = &rtPipelineFeatures;

		VK_CREATE_VALIDATION(vkCreateDevice(*physicalDevice, &createInfo, nullptr, &device), VkDevice);
		vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
	}
	void LogicalDevice::Destroy()
	{
		vkDestroyDevice(device, nullptr);
	}
	VkDevice LogicalDevice::GetDevice()
	{
		return device;
	}
	VkPhysicalDevice* LogicalDevice::GetPhysicalDevice()
	{
		return physicalDevice;
	}
	VkQueue LogicalDevice::GetGraphicsQueue()
	{
		return graphicsQueue;
	}
	VkQueue LogicalDevice::GetPresentQueue()
	{
		return presentQueue;
	}
}