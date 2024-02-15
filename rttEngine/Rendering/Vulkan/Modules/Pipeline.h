#pragma once
#include "Module.h"
#include "LogicalDevice.h"
#include <vector>
#include "Rendering\ShaderCompiler\Shader.h"
namespace rttvk {
	class Pipeline:public Module {
	public:
		Pipeline(Shader* shader, LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		VkPipeline GetPipeline();
		VkDescriptorSetLayout GetDescriptorLayout();
		VkPipelineLayout GetPipelineLayout();
	private:
		Shader* shader;
		LogicalDevice* device;

		VkPipelineLayout layout;
		VkPipelineCache cache;
		VkPipeline pipeline;
		VkDescriptorSetLayout descriptor;
	};
}