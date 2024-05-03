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

	VkDescriptorPoolSize poolSize3 = {};
	poolSize3.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSize3.descriptorCount = 1;

	VkDescriptorPoolSize poolSize4 = {};
	poolSize4.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize4.descriptorCount = 1;

	VkDescriptorPoolSize poolSize5 = {};
	poolSize5.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize5.descriptorCount = 1;

	VkDescriptorPoolSize poolSizes[] = { poolSize,poolSize2,poolSize3,poolSize4,poolSize5 };

	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = 5;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfo, nullptr, &descPool));

	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorPool = descPool;
	allocateInfo.descriptorSetCount = 1;
	auto layout = pipeline.GetDescriptorLayout();
	allocateInfo.pSetLayouts = &layout;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfo, &descSet));
	//buffer.Create();

	



	resolutionBuffer = rttvk::Buffer(&logicalDevice, sizeof(resolution), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	resolutionBuffer.Create();
	






	
	rayGenShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rgen",&logicalDevice,VK_SHADER_STAGE_RAYGEN_BIT_KHR);
	rayGenShader.Create();


	missShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rmiss", &logicalDevice, VK_SHADER_STAGE_MISS_BIT_KHR);
	missShader.Create();

	shadowMissShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracershadow.rmiss", &logicalDevice, VK_SHADER_STAGE_MISS_BIT_KHR);
	shadowMissShader.Create();

	closestHitShader = rttvk::Shader("Content/EngineLoad/Shaders/raytracer.rchit", &logicalDevice, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
	closestHitShader.Create();


	rtPipeline.Create();




	vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkCmdTraceRaysKHR");


	VkDescriptorPoolSize poolSizeRT = {};
	poolSizeRT.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizeRT.descriptorCount = 1;

	VkDescriptorPoolSize poolSizeRT2 = {};
	poolSizeRT2.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	poolSizeRT2.descriptorCount = 1;

	VkDescriptorPoolSize poolSizeRTRes = {};
	poolSizeRTRes.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizeRTRes.descriptorCount = 2;

	VkDescriptorPoolSize poolSizeRTNoise = {};
	poolSizeRTNoise.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizeRTNoise.descriptorCount = 1;

	VkDescriptorPoolSize poolSizeRTs[] = { poolSizeRT,poolSizeRT2,poolSizeRTRes,poolSizeRTNoise };

	VkDescriptorPoolCreateInfo poolCreateInfoRT = {};
	poolCreateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfoRT.poolSizeCount = 4;
	poolCreateInfoRT.pPoolSizes = poolSizeRTs;
	poolCreateInfoRT.maxSets = 1;

	VK_CREATE_VALIDATION(vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolCreateInfoRT, nullptr, &descPoolRT));

	VkDescriptorSetAllocateInfo allocateInfoRT = {};
	allocateInfoRT.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfoRT.descriptorPool = descPoolRT;
	allocateInfoRT.descriptorSetCount = 1;
	VkDescriptorSetLayout layoutRT = rtPipeline.GetDescriptorLayout();
	allocateInfoRT.pSetLayouts = &layoutRT;

	VK_CREATE_VALIDATION(vkAllocateDescriptorSets(logicalDevice.GetDevice(), &allocateInfoRT, &descSetRT));












	// Shader binding table
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

	// FIXME: Use better way to find sizes
	const uint32_t handleSize = rtProperties.shaderGroupHandleSize;
	const uint32_t handleSizeAligned = align(rtProperties.shaderGroupHandleSize, rtProperties.shaderGroupHandleAlignment);
	rgenRegion.size = handleSizeAligned;
	rgenRegion.stride = handleSizeAligned;
	rmissRegion.size = handleSizeAligned;
	rmissRegion.stride = handleSizeAligned;
	rchitRegion.size = handleSizeAligned;
	rchitRegion.stride = handleSizeAligned;
	
	shaderBindingTable = rttvk::Buffer(&logicalDevice, rgenRegion.size+ rmissRegion.size+ rchitRegion.size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR);
	shaderBindingTable.Create();
	
	

	uint32_t             dataSize = 4 * handleSizeAligned;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(logicalDevice.GetDevice(), "vkGetRayTracingShaderGroupHandlesKHR");
	vkGetRayTracingShaderGroupHandlesKHR(logicalDevice.GetDevice(), rtPipeline.GetPipeline(), 0, 4, dataSize, shaderBindingTable.GetMapped());


	rgenRegion.deviceAddress = shaderBindingTable.GetBufferAddress();
	rmissRegion.deviceAddress = shaderBindingTable.GetBufferAddress()+32;
	rchitRegion.deviceAddress = shaderBindingTable.GetBufferAddress() + 64+32;


	// Some test meshes to test how tlas works

	Mesh testMesh = Mesh();
	testMesh.pos[0] = 0;
	testMesh.pos[1] = 0;
	testMesh.pos[2] = 0;

	
	
	meshes = { testMesh };


	// Creating and building acceleration structures
	blasSponza.Create();
	blas.Create();
	tlas.Create();
	blasSponza.Build();
	blas.Build();
	tlas.Build();

	blueNoise.Create();

	// TODO: MAKE SIZE DYNAMIC
	rttGUIDrawHierarchy = rttvk::Buffer(&logicalDevice, 60*1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	rttGUIDrawHierarchy.Create();

	bufferHierarchyAmount = rttvk::Buffer(&logicalDevice, 4, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	bufferHierarchyAmount.Create();
	memcpy(bufferHierarchyAmount.GetMapped(), &rttGUI()->id, 4);

	font.Create();
	cameraPositionBuffer.Create();
}

VulkanLayer::~VulkanLayer()
{


	vkDeviceWaitIdle(logicalDevice.GetDevice());

	rttGUIDrawHierarchy.Destroy();
	bufferHierarchyAmount.Destroy();
	cameraPositionBuffer.Destroy();
	font.Destroy();

	blueNoise.Destroy();

	tlas.Destroy();
	blas.Destroy();

	shadowMissShader.Destroy();
	rayGenShader.Destroy();
	missShader.Destroy();
	closestHitShader.Destroy();

	shaderBindingTable.Destroy();
	rtPipeline.Destroy();



	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPoolRT, nullptr);



	resolutionBuffer.Destroy();

	//buffer.Destroy();

	//vkUnmapMemory(logicalDevice.GetDevice(), memory);
	//vkDestroyBuffer(logicalDevice.GetDevice(), resbuffer, nullptr);

	//vkDestroyDescriptorSet(logicalDevice.GetDevice(), descSet, nullptr);
	vkDestroyDescriptorPool(logicalDevice.GetDevice(), descPool, nullptr);
	inFlightFence.Destroy();
	imageAvailable.Destroy();
	renderFinished.Destroy();
	commandPool.Destroy();
	pipeline.Destroy();
	
	if (swapchain.isOk)
	{
		for (int i = 0; i < imageViews.size(); i++) {
			imageViews[i].Destroy();
		}
		swapchain.Destroy();
	}
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

	

	pos[0] = getProcessInfo()->camera[0][3];
	pos[1] = getProcessInfo()->camera[1][3];
	pos[2] = getProcessInfo()->camera[2][3];
	memcpy(rttGUIDrawHierarchy.GetMapped(), rttGUI()->drawHierarchy.data(), rttGUI()->drawHierarchy.size() * 4);
	memcpy(bufferHierarchyAmount.GetMapped(), &rttGUI()->id, 4);
	memcpy(cameraPositionBuffer.GetMapped(), pos, 12);




	// Ray tracing pipeline
	memcpy(resolutionBuffer.GetMapped(), resolution, sizeof(resolution));


	// TODO: Move this into constructor
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

	VkWriteDescriptorSetAccelerationStructureKHR wdsAc{};
	wdsAc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	wdsAc.accelerationStructureCount = 1;
	wdsAc.pAccelerationStructures = tlas.GetAccelerationStructure();

	VkWriteDescriptorSet wdsA = {};
	wdsA.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsA.dstSet = descSetRT;
	wdsA.dstBinding = 1;
	wdsA.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	wdsA.descriptorCount = 1;
	wdsA.pNext = &wdsAc;

	VkDescriptorBufferInfo resolutionInfo{};
	resolutionInfo.buffer = resolutionBuffer.GetBuffer();
	resolutionInfo.range = sizeof(resolution);


	VkWriteDescriptorSet wdsRes = {};
	wdsRes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsRes.dstSet = descSetRT;
	wdsRes.dstBinding = 2;
	wdsRes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsRes.descriptorCount = 1;
	wdsRes.pBufferInfo = &resolutionInfo;

	VkDescriptorImageInfo noiseImageInfo;
	noiseImageInfo.imageView = blueNoise.imageView.GetImageView();
	noiseImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	noiseImageInfo.sampler = blueNoise.textureSampler;

	VkWriteDescriptorSet wdsNoise = {};
	wdsNoise.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsNoise.dstSet = descSetRT;
	wdsNoise.dstBinding = 3;
	wdsNoise.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wdsNoise.descriptorCount = 1;
	wdsNoise.pImageInfo = &noiseImageInfo;

	VkDescriptorBufferInfo camPosInfo{};
	camPosInfo.buffer = cameraPositionBuffer.GetBuffer();
	camPosInfo.range = 16;


	VkWriteDescriptorSet wdsCamPos = {};
	wdsCamPos.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsCamPos.dstSet = descSetRT;
	wdsCamPos.dstBinding = 4;
	wdsCamPos.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsCamPos.descriptorCount = 1;
	wdsCamPos.pBufferInfo = &camPosInfo;

	VkWriteDescriptorSet wdss[] = { wds ,wdsA, wdsRes, wdsNoise,wdsCamPos };

	

	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 5, wdss, 0, nullptr);

	VkDescriptorBufferInfo amountInfo{};
	amountInfo.buffer = bufferHierarchyAmount.GetBuffer();
	amountInfo.range = 4;



	VkWriteDescriptorSet wdsAmount = {};
	wdsAmount.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsAmount.dstSet = descSet;
	wdsAmount.dstBinding = 2;
	wdsAmount.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	wdsAmount.descriptorCount = 1;
	wdsAmount.pBufferInfo = &amountInfo;

	VkDescriptorBufferInfo hierarchyInfo{};
	hierarchyInfo.buffer = rttGUIDrawHierarchy.GetBuffer();
	hierarchyInfo.range = rttGUI()->drawHierarchy.size()*4;


	VkWriteDescriptorSet wdsHierarchy = {};
	wdsHierarchy.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsHierarchy.dstSet = descSet;
	wdsHierarchy.dstBinding = 3;
	wdsHierarchy.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	wdsHierarchy.descriptorCount = 1;
	wdsHierarchy.pBufferInfo = &hierarchyInfo;

	wds.dstSet = descSet;

	wdsRes.dstSet = descSet;
	wdsRes.dstBinding = 1;

	VkDescriptorImageInfo fontImageInfo;
	fontImageInfo.imageView = font.imageView.GetImageView();
	fontImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	fontImageInfo.sampler = font.textureSampler;

	VkWriteDescriptorSet wdsFont = {};
	wdsFont.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wdsFont.dstSet = descSet;
	wdsFont.dstBinding = 4;
	wdsFont.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wdsFont.descriptorCount = 1;
	wdsFont.pImageInfo = &fontImageInfo;

	VkWriteDescriptorSet wdss2[] = { wds, wdsRes, wdsAmount, wdsHierarchy,wdsFont };
	vkUpdateDescriptorSets(logicalDevice.GetDevice(), 5, wdss2, 0, nullptr);



	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffer.GetBuffer(), &beginInfo);


	tlas.Update(&commandBuffer);

	ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline.GetPipelineLayout(), 0, 1, &descSetRT, 0, nullptr);

	VkStridedDeviceAddressRegionKHR sbt_null{};
	
	vkCmdTraceRaysKHR(commandBuffer.GetBuffer(), &rgenRegion, &rmissRegion, &rchitRegion, &sbt_null, resolution[0], resolution[1], 1);

	vkCmdBindPipeline(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipeline());
	vkCmdBindDescriptorSets(commandBuffer.GetBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.GetPipelineLayout(), 0, 1, &descSet, 0, nullptr);
	vkCmdDispatch(commandBuffer.GetBuffer(), resolution[0], resolution[1], 1);

	ChangeImageLayout(images[imageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	vkEndCommandBuffer(commandBuffer.GetBuffer());
}
void VulkanLayer::Draw()
{
	vkQueueWaitIdle(logicalDevice.GetGraphicsQueue());
	tlas.Recreate();
	// FIXME
	//RTT_LOG("NewFrame");
	//rttGUIDrawHierarchy.Destroy();
	//rttGUIDrawHierarchy = rttvk::Buffer(&logicalDevice,rttGUI()->drawHierarchy.size()*4+8,VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	//rttGUIDrawHierarchy.Create();

	vkWaitForFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP(), VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice.GetDevice(), 1, inFlightFence.GetFenceP());

	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), UINT64_MAX, imageAvailable.GetSemaphore(), inFlightFence.GetFence(), &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		Resize();
		return;
	}

	vkResetCommandBuffer(commandBuffer.GetBuffer(), 0);
	RecordCommandBuffer(imageIndex);
	VkCommandBuffer cb = commandBuffer.GetBuffer();

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

	VK_VALIDATE(vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, *inFlightFence.GetFenceP()), vkQueueSubmit);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapchains[] = { swapchain.GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;
	result = vkQueuePresentKHR(logicalDevice.GetPresentQueue(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		Resize();
	}
	ResetMeshes();
}

void VulkanLayer::Resize()
{
	vkDeviceWaitIdle(logicalDevice.GetDevice());
	swapchain.Destroy();
	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i].Destroy();
	}
	swapchain.Create();
	if (!swapchain.isOk)
	{
		return;
	}
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, nullptr);
	images.resize(imageCount);
	imageViews.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetDevice(), swapchain.GetSwapchain(), &imageCount, images.data());

	for (int i = 0; i < imageViews.size(); i++) {
		imageViews[i] = rttvk::ImageView(&logicalDevice, images[0]);
		imageViews[i].Create();
	}
}

