#include "VulkanLayer.h"

VulkanLayer::VulkanLayer()
{

	instance.Create();
	debugMessenger.Create();

	// Selecting Physical Device
	// TODO: IMPLEMENT BETTER SYSTEM
	uint32_t pdCount = 0;
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, nullptr);
	RTT_ASSERT(pdCount);
	std::vector<VkPhysicalDevice> physicalDevices(pdCount);
	vkEnumeratePhysicalDevices(instance.GetInstance(), &pdCount, physicalDevices.data());
	chosenPhysicalDevice = physicalDevices[0];

	logicalDevice.Create();

	surface = rttvk::SurfaceKHR(&instance, win64surface->Handle, win64surface->instance);
	surface.Create();
	swapchain.Create();

	// Get Image Count
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, nullptr);
	images.resize(imageCount);
	imageViews.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, images.data());

	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i] = rttvk::ImageView(&logicalDevice, images[0]);
		imageViews[i].Create();
	}

	computeShader = rttvk::Shader("Content/EngineLoad/Shaders/shader.comp", &logicalDevice, VK_SHADER_STAGE_COMPUTE_BIT);
	computeShader.Create();

	pipeline.Create();
	computeShader.Destroy();

	commandPool.Create();
	commandBuffer.Create();


	renderFinished.Create();
	imageAvailable.Create();
	inFlightFence.Create();

	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolSize poolSize2 = {};
	poolSize2.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize2.descriptorCount = 1;

	VkDescriptorPoolSize poolSizes[] = { poolSize,poolSize2 };

	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = 2;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfo, nullptr, &descPool),VkDescriptorPool);

	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descPool;
	allocateInfo.descriptorSetCount = 1;
	auto layout = pipeline.GetDescriptorLayout();
	allocateInfo.pSetLayouts = &layout;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfo, &descSet), VkDescriptorSet);
	//buffer.Create();










	RTT_LOG("-------- RAY TRACING --------");



	
	rayGenShader = rttvk::Shader("Content/EngineLoad/Shaders/rt.rgen",&logicalDevice,VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	rayGenShader.Create();

	missShader = rttvk::Shader("Content/EngineLoad/Shaders/rt.rmiss", &logicalDevice, VK_SHADER_STAGE_MISS_BIT_KHR);
	missShader.Create();

	closestHitShader = rttvk::Shader("Content/EngineLoad/Shaders/rt.rchit", &logicalDevice, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	closestHitShader.Create();


	rtPipeline.Create();




	vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkCmdTraceRaysKHR");


	VkDescriptorPoolSize poolSizeRT = {};
	poolSizeRT.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizeRT.descriptorCount = 1;

	VkDescriptorPoolSize poolSizeACRT = {};
	poolSizeACRT.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	poolSizeACRT.descriptorCount = 1;
	std::vector<VkDescriptorPoolSize> pools = { poolSizeRT,poolSizeACRT };

	VkDescriptorPoolCreateInfo poolCreateInfoRT = {};
	poolCreateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfoRT.poolSizeCount = pools.size();
	poolCreateInfoRT.pPoolSizes = pools.data();
	poolCreateInfoRT.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfoRT, nullptr, &descPoolRT), VkDescriptorPool);

	VkDescriptorSetAllocateInfo allocateInfoRT = {};
	allocateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfoRT.descriptorPool = descPoolRT;
	allocateInfoRT.descriptorSetCount = 1;
	VkDescriptorSetLayout layoutRT = rtPipeline.GetDescriptorLayout();
	allocateInfoRT.pSetLayouts = &layoutRT;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfoRT, &descSetRT), VkDescriptorSet);




	//rayGenBuffer.Create();









	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(chosenPhysicalDevice,
		&physicalDeviceProperties);

	rtProperties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};

	VkPhysicalDeviceProperties2 physicalDeviceProperties2 {
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
		&rtProperties,
		physicalDeviceProperties
		};

	vkGetPhysicalDeviceProperties2(chosenPhysicalDevice,
		&physicalDeviceProperties2);

	
	//uint32_t align = (physicalDeviceRayTracingPipelineProperties.shaderGroupHandleSize + physicalDeviceRayTracingPipelineProperties.shaderGroupHandleAlignment - 1) & ~(physicalDeviceRayTracingPipelineProperties.shaderGroupHandleAlignment - 1);

	uint32_t handleSizeAligned = align(rtProperties.shaderGroupHandleSize, rtProperties.shaderGroupHandleAlignment);










	rgenShaderBindingTable.size = align(handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	rgenShaderBindingTable.stride = rgenShaderBindingTable.size;

	missShaderBindingTable.size = align(1*handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	missShaderBindingTable.stride = handleSizeAligned;

	chitShaderBindingTable.size = align(1 * handleSizeAligned, rtProperties.shaderGroupBaseAlignment);
	chitShaderBindingTable.stride = handleSizeAligned;

	VkDeviceSize sbtSize = rgenShaderBindingTable.size;
	
	rayGenBuffer.Create();
	

	uint32_t             dataSize = 1 * rtProperties.shaderGroupHandleSize;
	std::vector<uint8_t> handles(dataSize);
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkGetRayTracingShaderGroupHandlesKHR");
	VK_VALIDATE(vkGetRayTracingShaderGroupHandlesKHR(logicalDevice.GetDevice(), rtPipeline.GetPipeline(), 0, 1, dataSize, rayGenBuffer.GetMapped()), vkGetRayTracingShaderGroupHandlesKHR);

	rgenShaderBindingTable.deviceAddress = rayGenBuffer.GetBufferAddress();
	missShaderBindingTable.deviceAddress = rayGenBuffer.GetBufferAddress() + rgenShaderBindingTable.size;
	chitShaderBindingTable.deviceAddress = rayGenBuffer.GetBufferAddress() + rgenShaderBindingTable.size + missShaderBindingTable.size;


	RTT_LOG("-------- BOTTOM LEVEL ACCELERATION STRUCTURES --------");
	blas1Buffer.Create();
	blas1.Create();
	RTT_LOG("-------- TOP LEVEL ACCELERATION STRUCTURES --------");
	tlasBuffer.Create();
	tlas.Create();







	
}

VulkanLayer::~VulkanLayer()
{

	RTT_LOG("Shutting down");


	vkDeviceWaitIdle(logicalDevice.GetDevice());

	rayGenBuffer.Destroy();
	rayGenShader.Destroy();
	rtPipeline.Destroy();









	//vkUnmapMemory(logicalDevice.GetDevice(), memory);
	//vkDestroyBuffer(logicalDevice.GetDevice(), resbuffer, nullptr);

	//vkDestroyDescriptorSet(logicalDevice.GetDevice(), descSet, nullptr);
	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPool, nullptr);
	inFlightFence.Destroy();
	imageAvailable.Destroy();
	renderFinished.Destroy();
	commandPool.Destroy();
	pipeline.Destroy();
	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i].Destroy();
	}
	swapchain.Destroy();
	surface.Destroy();
	logicalDevice.Destroy();
	debugMessenger.Destroy();
	instance.Destroy();
}

void VulkanLayer::ChangeImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageSubresourceRange access;
	access.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	access.baseMipLevel = 0;
	access.levelCount = 1;
	access.baseArrayLayer = 0;
	access.layerCount = 1;

	VkImageMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
	barrier.dstAccessMask = VK_ACCESS_NONE_KHR;
	barrier.image = image;
	barrier.subresourceRange = access;
	barrier.pNext = nullptr;


	vkCmdPipelineBarrier(commandBuffer.GetBuffer(), VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 0, 0, nullptr, 0, nullptr, 1, &barrier);

}

void VulkanLayer::RecordCommandBuffer(uint32_t imageIndex)
{

	RTT_LOG("Descriptors ----------------------------------------------------");
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView = imageViews[imageIndex].GetImageView();
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet wds = {};
	wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds.dstSet = descSetRT;
	wds.dstBinding = 0;
	wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	wds.descriptorCount = 1;

	wds.pImageInfo = &imageInfo;


	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 1, &wds, 0, nullptr);

	VkWriteDescriptorSet wdsAC = {};
	wdsAC.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsAC.dstSet = descSetRT;
	wdsAC.dstBinding = 1;
	wdsAC.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	wdsAC.descriptorCount = 1;


	VkAccelerationStructureKHR ass[] = { tlas.GetAS() };

	VkWriteDescriptorSetAccelerationStructureKHR accS{};
	accS.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	accS.accelerationStructureCount = 1;
	accS.pAccelerationStructures = ass;

	wdsAC.pNext = &accS;

	RTT_LOG("Command Buffer ----------------------------------------------------");
	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 1, &wdsAC, 0, nullptr);


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//RTT_LOG("\t\tCommand Buffer");
	vkBeginCommandBuffer(commandBuffer.GetBuffer(), &beginInfo);
	blas1.Build(&commandBuffer);
	tlas.Build(&commandBuffer);


	ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipelineLayout(), 0, 1, &descSetRT, 0, nullptr);


	VkStridedDeviceAddressRegionKHR sbt_null{};
	
	vkCmdTraceRaysKHR(commandBuffer.GetBuffer(), &rgenShaderBindingTable, &missShaderBindingTable, &chitShaderBindingTable, &sbt_null, 1280, 720, 1);

	ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//RTT_LOG("\t\tCommand Buffer END");
	vkEndCommandBuffer(commandBuffer.GetBuffer());
}
void VulkanLayer::Draw()
{ 
	
	//RTT_LOG("\t\tNew Frame");
	vkWaitForFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP(), VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP());
	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, imageAvailable.GetSemaphore(), inFlightFence.GetFence(), &imageIndex);
	//RTT_LOG("\t\t\tCurrentImage: "+ std::to_string(imageIndex));

	vkResetCommandBuffer(commandBuffer.GetBuffer(), 0);
	RecordCommandBuffer(imageIndex);
	VkCommandBuffer cb = commandBuffer.GetBuffer();

	RTT_LOG("Submit ----------------------------------------------------");
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { imageAvailable.GetSemaphore() };
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cb;
	VkSemaphore signalSemaphores[] = { renderFinished.GetSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VK_VALIDATE(vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, inFlightFence.GetFence()), vkQueueSubmit);

	//VK_VALIDATE(vkWaitForFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP(), VK_TRUE, UINT64_MAX), vkWaitForFences);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapchains[] = { swapchain.GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	VkResult result = vkQueuePresentKHR(logicalDevice.GetPresentQueue(), &presentInfo);
}

