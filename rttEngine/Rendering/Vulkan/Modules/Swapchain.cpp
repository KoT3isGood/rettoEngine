#include "Swapchain.h"
namespace rttvk {
	Swapchain::Swapchain(SurfaceKHR* surface,LogicalDevice* device)
	{
		this->device = device;
		this->surface = surface;
	}
	void Swapchain::Create()
	{
		VkSurfaceCapabilitiesKHR capabilies;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device->GetPhysicalDevice(), surface->GetSurface(), &capabilies);


		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface->GetSurface();
		createInfo.minImageCount = 1;
		createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = capabilies.minImageExtent;
		if (createInfo.imageExtent.height > INT16_MAX || createInfo.imageExtent.width > UINT16_MAX || createInfo.imageExtent.height < 0 || createInfo.imageExtent.width < 0)
		{
			isOk = false;
			return;
		}
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		VK_CREATE_VALIDATION(vkCreateSwapchainKHR(device->GetDevice(), &createInfo, nullptr, &swapchain));

	}

	void Swapchain::Destroy()
	{
		vkDestroySwapchainKHR(device->GetDevice(), swapchain, nullptr);
	}
	VkSwapchainKHR Swapchain::GetSwapchain()
	{
		return swapchain;
	}
}