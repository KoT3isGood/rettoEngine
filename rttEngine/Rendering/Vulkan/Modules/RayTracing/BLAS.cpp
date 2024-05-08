#include "BLAS.h"
namespace rttvk {
	BLAS::BLAS(LogicalDevice* device, MeshData* meshData)
	{
		this->device = device;
		this->meshData = meshData;
		
	}
	void BLAS::Create()
	{
		vertexBuffer = Buffer(device, meshData->vertices.size()*4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		);
		indexBuffer = Buffer(device, meshData->indicies.size() * 4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);

		uvBuffer = Buffer(device, meshData->uvs.size() * 4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);
		uvIndexesBuffer = Buffer(device, meshData->uvIndicies.size() * 4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);

		std::vector<Material> materials = {};
		for (auto mat : meshData->materialsArray) {
			materials.push_back(mat.second);
		}
		materialBuffer = Buffer(device, materials.size()*sizeof(Material)+4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);
		materialIndexesBuffer = Buffer(device, meshData->materialSizes.size() * 4+4,
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR |
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);

		vertexBuffer.Create();
		//vertexBuffer.shouldBeMapped = false;
		indexBuffer.Create();

		uvBuffer.Create();
		uvIndexesBuffer.Create();
		materialBuffer.Create();
		materialIndexesBuffer.Create();
		
		memcpy(vertexBuffer.GetMapped(), meshData->vertices.data(), meshData->vertices.size() * 4);
		memcpy(indexBuffer.GetMapped(), meshData->indicies.data(), meshData->indicies.size() * 4);
		memcpy(uvBuffer.GetMapped(), meshData->uvs.data(), meshData->uvs.size() * 4);
		memcpy(uvIndexesBuffer.GetMapped(), meshData->uvIndicies.data(), meshData->uvIndicies.size() * 4);
		memcpy(materialBuffer.GetMapped(), materials.data(), materials.size() * sizeof(Material));
		memcpy(materialIndexesBuffer.GetMapped(), meshData->materialSizes.data(), meshData->materialSizes.size() * 4);

		triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		triangles.vertexData.deviceAddress = vertexBuffer.GetBufferAddress();
		triangles.vertexStride = 12;
		triangles.maxVertex = meshData->vertices.size()/3;
		triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress = indexBuffer.GetBufferAddress();
		//triangles.indexData.deviceAddress = indexBuffer.GetBufferAddress();
		
		asGeom.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		asGeom.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		asGeom.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		asGeom.geometry.triangles = triangles;

		offset.firstVertex = 0;
		offset.primitiveCount = meshData->indicies.size()/3;
		offset.primitiveOffset = 0;
		offset.transformOffset = 0;

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		geometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		geometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_DATA_ACCESS_KHR | VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		geometryInfo.pGeometries = &asGeom;
		geometryInfo.geometryCount = 1;


		


		VK_FUNCTION(vkCreateAccelerationStructureKHR, device->GetDevice());
		VK_FUNCTION(vkGetAccelerationStructureBuildSizesKHR, device->GetDevice());

		uint32_t count = meshData->vertices.size();

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &geometryInfo, &count, &sizeInfo);


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
		const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &offset;
		vkCmdBuildAccelerationStructuresKHR(cmd.GetBuffer(), 1, &geometryInfo, &pBuildOffsetInfo);
		vkEndCommandBuffer(cmd.GetBuffer());

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd.GetBufferP();

		float start = getRunningTime();
		vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, nullptr);
		vkQueueWaitIdle(device->GetGraphicsQueue());
		float end = getRunningTime();

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
