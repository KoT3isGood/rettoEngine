#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{
	CreateInstance();
	
}

VulkanLayer::~VulkanLayer()
{
	vkDestroyInstance(instance,nullptr);
}

void VulkanLayer::Draw()
{

}

void VulkanLayer::CreateInstance()
{
	

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(0, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	std::vector<char*> extensionNames(extensionCount);
	vkEnumerateInstanceExtensionProperties(0, &extensionCount, extensions.data());


	RTT_LOG("There are "+ std::to_string(extensionCount) + " extensions available:");
	for (uint32_t i = 0; i < extensionCount; i++) {
		extensionNames[i] = extensions[i].extensionName;
		RTT_LOG("   "+ std::string(extensions[i].extensionName));
	}
	
	


	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = getProcessInfo()->appInfo->name;
	appInfo.pEngineName = "rtt";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 1);
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.pNext = nullptr;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
	if (createResult == VK_SUCCESS) {
		RTT_LOG("Instance Created Successfuly");
		return;
	}
	RTT_LOG("Failed to create instance");
}
