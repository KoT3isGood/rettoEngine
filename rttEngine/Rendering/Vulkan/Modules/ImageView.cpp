#include "ImageView.h"
namespace rttvk {
	ImageView::ImageView(LogicalDevice* device, VkImage image)
	{
		this->image = image;
		this->device = device;
		this->format = VK_FORMAT_R8G8B8A8_UNORM;
	}
	ImageView::ImageView(LogicalDevice* device, VkImage image, VkFormat format)
	{
		this->image = image;
		this->device = device;
		this->format = format;
	}
	void ImageView::Create()
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VK_CREATE_VALIDATION(vkCreateImageView(device->GetDevice(),&createInfo,nullptr,&imageView));
	}
	void ImageView::Destroy()
	{
		vkDestroyImageView(device->GetDevice(), imageView, nullptr);
	}
	VkImageView ImageView::GetImageView()
	{
		return imageView;
	}
}