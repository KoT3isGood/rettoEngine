#pragma once
#include "Module.h"
#include "LogicalDevice.h"

namespace rttvk {
	class ImageView :public Module {
	public:
		ImageView() = default;
		ImageView(LogicalDevice* device, VkImage image);
		virtual void Create() override;
		virtual void Destroy() override;
		VkImageView GetImageView();
	private:
		VkImage image;
		LogicalDevice* device;
		VkImageView imageView;
	};
}