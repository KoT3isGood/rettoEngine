#include "BLAS.h"

namespace rttvk {
	BLAS::BLAS(LogicalDevice* device)
	{
		this->device = device;
	}
	void BLAS::Create()
	{
		DEVICE_FUNCTION(device, vkCreateAccelerationStructureKHR);
		DEVICE_FUNCTION(device, vkGetAccelerationStructureBuildSizesKHR);
		
		// Define BLAS vertices
		float quadVertices[] = {
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f
		};

		// Define BLAS indicies
		uint32_t quadIndices[] = {
			0, 1, 2, 2, 1, 3
		};

		vertexBuffer = Buffer(device, sizeof(quadVertices), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		vertexBuffer.Create();
		vertexBuffer.SetData(quadVertices,sizeof(quadVertices));

		indexBuffer = Buffer(device, sizeof(quadVertices), VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		indexBuffer.Create();
		vertexBuffer.SetData(quadIndices, sizeof(quadIndices));

		


		geometryBLAS.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometryBLAS.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometryBLAS.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		geometryBLAS.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		geometryBLAS.geometry.triangles.vertexData.deviceAddress = vertexBuffer.GetBufferAddress();
		geometryBLAS.geometry.triangles.vertexStride = 3 * sizeof(float); // Each vertex has 3 floats
		geometryBLAS.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		geometryBLAS.geometry.triangles.indexData.deviceAddress = indexBuffer.GetBufferAddress();

		buildInfoBLAS.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfoBLAS.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildInfoBLAS.geometryCount = 1;
		buildInfoBLAS.pGeometries = &geometryBLAS;
		buildInfoBLAS.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;

		VkAccelerationStructureCreateInfoKHR blasCreateInfo = {};
		blasCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		blasCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		

		uint32_t size = 2;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo{};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		vkGetAccelerationStructureBuildSizesKHR(device->GetDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_HOST_KHR, &buildInfoBLAS, &size, &sizeInfo);


		blasBuffer = Buffer(device, sizeInfo.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		blasBuffer.Create();
		scratchBuffer = Buffer(device, 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		if (sizeInfo.buildScratchSize>0) {
			scratchBuffer = Buffer(device, sizeInfo.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		}

		RTT_LOG("accelerationStructureSize: " + std::to_string(sizeInfo.accelerationStructureSize));
		RTT_LOG("buildScratchSize: " + std::to_string(sizeInfo.buildScratchSize));

		scratchBuffer.Create();
		buildInfoBLAS.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		blasCreateInfo.size = sizeInfo.accelerationStructureSize;
		blasCreateInfo.buffer = blasBuffer.GetBuffer();


		vkCreateAccelerationStructureKHR(device->GetDevice(), &blasCreateInfo, nullptr, &blas);

		buildInfoBLAS.srcAccelerationStructure = blas;
		buildInfoBLAS.dstAccelerationStructure = blas;

		buildRangeInfo.primitiveCount = 2;
		buildRangeInfo.primitiveOffset = 9;
		buildRangeInfo.firstVertex = 0;
		buildRangeInfo.transformOffset = 0;
	}
	void BLAS::Destroy()
	{
	}
	void BLAS::Build(CommandBuffer* commandBuffer)
	{
		DEVICE_FUNCTION(device, vkCmdBuildAccelerationStructuresKHR);

		VkAccelerationStructureBuildRangeInfoKHR* buildRangeInfos[] = { &buildRangeInfo };
		vkCmdBuildAccelerationStructuresKHR(commandBuffer->GetBuffer(), 1, &buildInfoBLAS, buildRangeInfos);
	}
	uint64_t BLAS::GetAddress()
	{
		DEVICE_FUNCTION(device, vkGetAccelerationStructureDeviceAddressKHR);

		VkAccelerationStructureDeviceAddressInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		info.accelerationStructure = blas;
		return vkGetAccelerationStructureDeviceAddressKHR(device->GetDevice(), &info);
	}
}

