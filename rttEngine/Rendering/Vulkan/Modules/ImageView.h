#pragma once
#include "Module.h"
#include "LogicalDevice.h"

namespace rttvk {
	class ImageView :public Module {
	public:
		ImageView() = default;
		ImageView(LogicalDevice* device, VkImage image);
		ImageView(LogicalDevice* device, VkImage image, VkFormat format);
		virtual void Create() override;
		virtual void Destroy() override;
		VkImageView GetImageView();
		VkFormat format;
	private:
		VkImage image;
		LogicalDevice* device;
		VkImageView imageView;
	};
}