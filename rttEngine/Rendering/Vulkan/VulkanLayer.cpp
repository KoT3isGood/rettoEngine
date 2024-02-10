#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{
	
	instance.Create();
	debugMessenger.Create();


	uint32_t pdCount = 0;
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, nullptr);
	RTT_ASSERT(pdCount);
	std::vector<VkPhysicalDevice> physicalDevices(pdCount);
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, physicalDevices.data());
	chosenPhysicalDevice = physicalDevices[0]; // TODO: IMPLEMENT BETTER SYSTEM

	logicalDevice.Create();
}

VulkanLayer::~VulkanLayer()
{
	logicalDevice.Destroy();
	debugMessenger.Destroy();
	instance.Destroy();
}

void VulkanLayer::Draw()
{
}
