#include "BLAS.h"
namespace rttvk {
	BLAS::BLAS(LogicalDevice* device)
	{
		this->device = device;

		
	}
	void BLAS::Create()
	{
		uint32_t amountOfTriangles = 1;

		float trianglesA[] = {
			10,0,0,
			10,0,1,
			10,1,0
		};
		uint32_t indicies[] = {
			1,2,3,
			1,2,0
		};
		RTT_LOG("Buffer size:" + std::to_string(sizeof(trianglesA)));
		vertexBuffer = Buffer(device, sizeof(trianglesA),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		);
		indexBuffer = Buffer(device, sizeof(indicies),
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);

		vertexBuffer.Create();
		indexBuffer.Create();

		
		memcpy(vertexBuffer.GetMapped(), trianglesA, sizeof(trianglesA));
		memcpy(indexBuffer.GetMapped(), indicies, sizeof(indicies));

		triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		triangles.vertexData.deviceAddress = vertexBuffer.GetBufferAddress();
		triangles.vertexStride = 12;
		triangles.maxVertex = 3;
		triangles.indexType = VK_INDEX_TYPE_NONE_KHR;
		//triangles.indexData.deviceAddress = indexBuffer.GetBufferAddress();
		
		asGeom.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeom.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		asGeom.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		asGeom.geometry.triangles = triangles;

		offset.firstVertex = 0;
		offset.primitiveCount = amountOfTriangles;
		offset.primitiveOffset = 0;
		offset.transformOffset = 0;

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		geometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		geometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		geometryInfo.pGeometries = &asGeom;
		geometryInfo.geometryCount = 1;


		


		VK_FUNCTION(vkCreateAccelerationStructureKHR, device->GetDevice());
		VK_FUNCTION(vkGetAccelerationStructureBuildSizesKHR, device->GetDevice());

		uint32_t count = 2;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &geometryInfo, &count, &sizeInfo);

		RTT_LOG("[ VULKAN ] [ BLAS ] Build Sizes");
		RTT_LOG("[ VULKAN ] [ BLAS ] Acceleration structure size: "+std::to_string(sizeInfo.accelerationStructureSize));
		RTT_LOG("[ VULKAN ] [ BLAS ] Scratch size: " + std::to_string(sizeInfo.buildScratchSize));

		scratchBuffer = Buffer(device, sizeInfo.buildScratchSize,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
		);
		scratchBuffer.Create();
		geometryInfo.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		acBuffer = Buffer(device, sizeInfo.accelerationStructureSize,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
		);
		acBuffer.Create();

		VkAccelerationStructureCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.buffer = acBuffer.GetBuffer();
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

		VK_CREATE_VALIDATION(vkCreateAccelerationStructureKHR(device->GetDevice(), &createInfo, nullptr, &blas));

		geometryInfo.dstAccelerationStructure = blas;
	}
	void BLAS::Destroy()
	{
		VK_FUNCTION(vkDestroyAccelerationStructureKHR, device->GetDevice());
		vkDestroyAccelerationStructureKHR(device->GetDevice(), blas, nullptr);

		vertexBuffer.Destroy();
		indexBuffer.Destroy();
		acBuffer.Destroy();
		scratchBuffer.Destroy();
	}
	void BLAS::Build()
	{
		cmdPool = CommandPool(device);
		cmdPool.Create();
		cmd = CommandBuffer(device, &cmdPool);
		cmd.Create();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(cmd.GetBuffer(), &beginInfo);
		VK_FUNCTION(vkCmdBuildAccelerationStructuresKHR, device->GetDevice());
		VkAccelerationStructureBuildRangeInfoKHR        buildOffsetInfo{ 1, 0, 0, 0 };
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;
		vkCmdBuildAccelerationStructuresKHR(cmd.GetBuffer(), 1, &geometryInfo, &pBuildOffsetInfo);
		vkEndCommandBuffer(cmd.GetBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd.GetBufferP();

		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, nullptr);
		vkQueueWaitIdle(device->GetGraphicsQueue());

		cmd.Destroy();
		cmdPool.Destroy();
	}
	VkAccelerationStructureKHR* BLAS::GetAccelerationStructure()
	{
		return &blas;
	}
	VkDeviceAddress BLAS::GetAccelerationStructureAddress()
	{
		VK_FUNCTION(vkGetAccelerationStructureDeviceAddressKHR, device->GetDevice());

		VkAccelerationStructureDeviceAddressInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		info.accelerationStructure = blas;
		return vkGetAccelerationStructureDeviceAddressKHR(device->GetDevice(), &info);
	}
}