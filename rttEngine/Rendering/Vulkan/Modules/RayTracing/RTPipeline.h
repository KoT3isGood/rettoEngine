#pragma once
#include "..\LogicalDevice.h";
#include "Rendering\ShaderCompiler\Shader.h"
namespace rttvk {
	class RTPipeline: public Module {
	public:
		RTPipeline(LogicalDevice* device, std::vector<Shader*> shaders, std::vector<VkDescriptorSetLayoutBinding> descSetlayout);
		virtual void Create() override;
		virtual void Destroy() override;
		VkPipeline GetPipeline();
		VkDescriptorSetLayout GetDescriptorLayout();
		VkPipelineLayout GetPipelineLayout();
	private:
		std::vector<VkDescriptorSetLayoutBinding> descSetlayout;
		LogicalDevice* device;
		std::vector<Shader*> shaders;


		VkPipelineLayout layout;
		VkPipeline pipeline;
		VkDescriptorSetLayout descriptor;

	private:
		PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
	};
}