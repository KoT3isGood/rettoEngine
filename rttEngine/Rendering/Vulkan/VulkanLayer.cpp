#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{
#ifdef ToolChain
	validationLayers = true;
#endif
	CreateInstance();
	if (validationLayers) {
		RTT_LOG("Validation Layers enabled, creating them");
		CreateValidationLayers();
	}

	SelectPhysicalDevice();
}

VulkanLayer::~VulkanLayer()
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func) {
		func(instance, messenger, nullptr);
	}
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


	for (uint32_t i = 0; i < extensionCount; i++) {
		extensionNames[i] = extensions[i].extensionName;
	}
	std::vector<const char*> layers;
	if (validationLayers) {
		layers.push_back("VK_LAYER_KHRONOS_validation");
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
	createInfo.enabledLayerCount = uint32_t(layers.size());
	createInfo.ppEnabledLayerNames = layers.data();
	createInfo.pNext = nullptr;

	VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
	if (createResult == VK_SUCCESS) {
		//RTT_LOG("Instance Created Successfuly");
		return;
	}
	//RTT_LOG("Failed to create instance");
}
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLayer::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
	VkDebugUtilsMessageTypeFlagsEXT message_types, 
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data, 
	void* user_data)
{
	RTT_LOG(callback_data->pMessage);
	return VK_FALSE;
}

void VulkanLayer::CreateValidationLayers()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func) {
		func(instance, &createInfo, nullptr, &messenger);
	}
	else {
		//RTT_LOG("Failed to create validation layers");
	}
}


void VulkanLayer::SelectPhysicalDevice()
{
	// Criteria: Ray Tracing Pipeline
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	if (!physicalDeviceCount) {
		RTT_LOG("Vulkan: Oh no, there are no connected GPU's!");
		RTT_ASSERT(1)
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	bool foundAnything = false;
	VkPhysicalDevice candidate = VkPhysicalDevice();

	for (auto candidatePhysicalDevice : physicalDevices) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(candidatePhysicalDevice, &properties);

		bool supportsRayTracing = false;

		uint32_t deviceExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(candidatePhysicalDevice,nullptr, &deviceExtensionCount,nullptr);
		std::vector <VkExtensionProperties> deviceExtensions(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(candidatePhysicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());

		for (const auto deviceExtension : deviceExtensions) {
			if (std::string(deviceExtension.extensionName) == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) {
				foundAnything = true;
				candidate = candidatePhysicalDevice;
			};
		};
	}
	if (!foundAnything) {
		RTT_LOG("Vulkan: Oh no, there are no devices with VK_KHR_ray_tracing_pipeline support");
		RTT_LOG("You can use -gl parameter in launch options");
		RTT_ASSERT(1)
	};

	physicalDevice = candidate;
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	RTT_LOG("Selected GPU: "+ std::string(properties.deviceName));

}