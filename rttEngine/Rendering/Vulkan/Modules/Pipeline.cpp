#include "Pipeline.h"

namespace rttvk{
	Pipeline::Pipeline(Shader* shader, LogicalDevice* device, std::vector<VkDescriptorSetLayoutBinding> descSetlayout)
	{
		
		this->shader = shader;
		this->device = device;
		this->descSetlayout = descSetlayout;
	}
	void Pipeline::Create()
	{
		VkDescriptorSetLayoutCreateInfo descriptorInfo{};
		descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorInfo.bindingCount = descSetlayout.size();
		descriptorInfo.pBindings = descSetlayout.data();

		VK_CREATE_VALIDATION(vkCreateDescriptorSetLayout(device->GetDevice(), &descriptorInfo, nullptr, &descriptor));

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pSetLayouts = &descriptor;
		pipelineLayoutInfo.setLayoutCount = 1;
		VK_CREATE_VALIDATION(vkCreatePipelineLayout(device->GetDevice(), &pipelineLayoutInfo, nullptr, &layout));




		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.layout = layout;
		createInfo.stage = shader->shaderStageInfo;


		VK_CREATE_VALIDATION(vkCreateComputePipelines(device->GetDevice(), nullptr, 1, &createInfo, nullptr, &pipeline));
	}
	void Pipeline::Destroy()
	{
		
		vkDestroyPipeline(device->GetDevice(), pipeline, nullptr);
		vkDestroyPipelineLayout(device->GetDevice(), layout, nullptr);
		vkDestroyDescriptorSetLayout(device->GetDevice(), descriptor, nullptr);
	}
	VkPipeline Pipeline::GetPipeline()
	{
		return pipeline;
	}
	VkDescriptorSetLayout Pipeline::GetDescriptorLayout()
	{
		return descriptor;
	}
	VkPipelineLayout Pipeline::GetPipelineLayout()
	{
		return layout;
	}
}

