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

	CreateSurface();

	GetQueues();

	CreateLogicalDevice();

	CreateSwapchain();

	CreateImageViews();
}

VulkanLayer::~VulkanLayer()
{
	for (auto imageView : swImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
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
	
	// Extensions
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

	// App Info
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = getProcessInfo()->appInfo->name;
	appInfo.pEngineName = "rtt";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 1);
	appInfo.apiVersion = VK_API_VERSION_1_3;
	appInfo.pNext = nullptr;


	// Create info
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = uint32_t(layers.size());
	createInfo.ppEnabledLayerNames = layers.data();
	createInfo.pNext = nullptr;

	// Create it
	VkResult createResult = vkCreateInstance(&createInfo, nullptr, &instance);
	if (createResult == VK_SUCCESS) {
		return;
	}
	RTT_LOG("Failed to create instance");
	RTT_ASSERT(0);
}
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLayer::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
	VkDebugUtilsMessageTypeFlagsEXT message_types, 
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data, 
	void* user_data)
{
	// Print debug information
	RTT_LOG(callback_data->pMessage);
	return VK_FALSE;
}

void VulkanLayer::CreateValidationLayers()
{
	// Create info
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	// Create it
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
	// Get all devices
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	if (!physicalDeviceCount) {
		RTT_LOG("Vulkan: Oh no, there are no connected GPU's!");
		RTT_ASSERT(0);
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	bool foundAnything = false;
	VkPhysicalDevice candidate = VkPhysicalDevice();


	// Loop
	for (auto candidatePhysicalDevice : physicalDevices) {
		// Get all extensions of current device
		uint32_t deviceExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(candidatePhysicalDevice,nullptr, &deviceExtensionCount,nullptr);
		std::vector <VkExtensionProperties> deviceExtensions(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(candidatePhysicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());
		// Loop
		for (const auto deviceExtension : deviceExtensions) {
			// Check if there is VK_KHR_ray_tracing_pipeline support
			if (std::string(deviceExtension.extensionName) == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) {
				foundAnything = true;
				candidate = candidatePhysicalDevice;
			};
		};
	}
	if (!foundAnything) {
		RTT_LOG("Vulkan: Oh no, there are no devices with VK_KHR_ray_tracing_pipeline support");
		RTT_LOG("You can use -gl parameter in launch options to open instance with OpenGL");
		RTT_ASSERT(0);
	};

	physicalDevice = candidate;
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	RTT_LOG("Selected GPU: "+ std::string(properties.deviceName));

}

void VulkanLayer::GetQueues()
{
	// Get Queue
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

		if (presentSupport) {
			presentFamily = i;
		}
		if (graphicsFamily != -1 && presentFamily!=-1) {
			break;
		}

		i++;
	}
}

void VulkanLayer::CreateSurface()
{
#ifdef  Win64
	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	Win64Surface* win64surface = dynamic_cast<Win64Surface*>(getProcessInfo()->surface);
	createInfo.hwnd = win64surface->Handle;
	createInfo.hinstance = win64surface->instance;
	
	VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
	if (result == VK_SUCCESS)
	{
		return;
	}
	RTT_LOG("Vulkan: Oh no, failed to create vulkan surface!");
	RTT_ASSERT(0);
#endif

}

void VulkanLayer::CreateSwapchain()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = capabilities.minImageCount+1;
	createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;



	VkExtent2D actualExtent = {
		   static_cast<uint32_t>(getProcessInfo()->surface->resolutionX),
		   static_cast<uint32_t>(getProcessInfo()->surface->resolutionY)
	};

	actualExtent.width = min(mathMax(actualExtent.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
	actualExtent.height = min(mathMax(actualExtent.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);
	createInfo.imageExtent = actualExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	createInfo.queueFamilyIndexCount = 1;
	createInfo.pQueueFamilyIndices = &graphicsFamily;
	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(logicalDevice,&createInfo,nullptr,&swapchain);
	if (result != VK_SUCCESS)
	{

		RTT_LOG("Vulkan: Oh no, failed to create vulkan surface!");
		RTT_ASSERT(0);
	}

	uint32_t amount;
	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &amount, nullptr);
	swImages.resize(amount);
	swImageViews.resize(amount);
	vkGetSwapchainImagesKHR(logicalDevice, swapchain, &amount, swImages.data());
}

void VulkanLayer::CreateImageViews()
{
	for (int i = 0; i < swImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		vkCreateImageView(logicalDevice, &createInfo, nullptr, &swImageViews[i]);
	}
}

void VulkanLayer::CreateLogicalDevice()
{
	std::vector<const char*> extensions = {
		VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
		VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
		VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	

	const float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;



	createInfo.enabledLayerCount = 0;

	createInfo.enabledExtensionCount = uint32_t(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	VkResult createResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
	if (createResult != VK_SUCCESS) {
		RTT_LOG("Failed to create logical device");
		RTT_ASSERT(0);

	}
}

