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
		float priority = 1.0; // Should Always be 1.0
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = 0;
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

		VK_CREATE_VALIDATION(vkCreateDevice(*physicalDevice,&createInfo,nullptr,&device), VkDevice)
	}
	void LogicalDevice::Destroy()
	{
		vkDestroyDevice(device, nullptr);
	}
	VkDevice LogicalDevice::GetDevice()
	{
		return device;
	}
}