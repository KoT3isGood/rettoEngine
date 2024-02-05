#pragma once
#include "Runtime\Layers\RenderingLayer.h"
#include "Vulkan\vulkan.h"
#include <vector>
#include "Runtime\Application\ProcessInfo.h"

class VulkanLayer : public RenderingLayer {
public:
	VulkanLayer();
	~VulkanLayer();
	virtual void Draw() override;
private:
	//~Instance
	VkInstance instance;

	void CreateInstance();
	//~Instance
};