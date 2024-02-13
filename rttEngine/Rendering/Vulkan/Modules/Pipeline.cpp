#include "Pipeline.h"

namespace rttvk{
	Pipeline::Pipeline(Shader* shader, LogicalDevice* device)
	{
		this->shader = shader;
		this->device = device;
	}
	void Pipeline::Create()
	{
		

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo.setLayoutCount = 1;
		//pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;
		VK_CREATE_VALIDATION(vkCreatePipelineLayout(device->GetDevice(), &pipelineLayoutInfo, nullptr, &layout), VkPipelineLayout);



		VkPipelineCacheCreateInfo cacheCreateInfo{};
		cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CREATE_VALIDATION(vkCreatePipelineCache(device->GetDevice(), &cacheCreateInfo, nullptr, &cache), VkPipelineCache);

		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.layout = layout;
		createInfo.stage = shader->shaderStageInfo;


		VK_CREATE_VALIDATION(vkCreateComputePipelines(device->GetDevice(), cache, 1, &createInfo, nullptr, &pipeline), VkPipeline);
	}
	void Pipeline::Destroy()
	{
		
		vkDestroyPipeline(device->GetDevice(), pipeline, nullptr);
		vkDestroyPipelineCache(device->GetDevice(), cache, nullptr);
		vkDestroyPipelineLayout(device->GetDevice(), layout, nullptr);
	}
	VkPipeline Pipeline::GetPipeline()
	{
		return pipeline;
	}
}

