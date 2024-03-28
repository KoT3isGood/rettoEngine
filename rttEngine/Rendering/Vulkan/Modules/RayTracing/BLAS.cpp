#include "BLAS.h"

namespace rttvk {
	BLAS::BLAS(LogicalDevice* device, Buffer* buffer)
	{
		this->device = device;
		this->buffer = buffer;
	}
	void BLAS::Create()
	{
		DEVICE_FUNCTION(device, vkCreateAccelerationStructureKHR);
		
		


		VkAccelerationStructureCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		createInfo.size = 32+48*2+12+64;
		createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		createInfo.buffer = buffer->GetBuffer(); 
		VK_CREATE_VALIDATION(vkCreateAccelerationStructureKHR(device->GetDevice(), &createInfo, nullptr, &structure), VkAccelerationStructureKHR);

		float verticesData[12] = {
			0,1,1,0,
			0,1,-1,0,
			0,-1,1,0,
		};
		int indiciesData[3] = {
			0,1,2
		};
		float transformMatrix[3][4] = { {
			1,0,0,0,
		}, {
			0,1,0,0,
		}, {
			0,0,1,0,
		}};
		vertices = Buffer(device, 48, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		indicies = Buffer(device, 12, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		transform = Buffer(device, 48, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		scratchBuffer = Buffer(device, 64, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
		vertices.Create();
		memcpy(vertices.GetMapped(), verticesData, 48);
		indicies.Create();
		memcpy(indicies.GetMapped(), indiciesData, 12);
		transform.Create();
		memcpy(transform.GetMapped(), transformMatrix, 48);
		scratchBuffer.Create();

		VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
		triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		triangles.vertexFormat = VK_FORMAT_R64G64B64A64_SFLOAT;
		triangles.vertexData.deviceAddress = vertices.GetBufferAddress();
		triangles.vertexStride = 12;
		triangles.maxVertex = 3;
		triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress = indicies.GetBufferAddress();
		triangles.transformData.deviceAddress = transform.GetBufferAddress();

		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometry.geometry.triangles = triangles;
		geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

		geometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		geometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		geometryInfo.flags = 0;
		geometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		geometryInfo.srcAccelerationStructure = VK_NULL_HANDLE;
		geometryInfo.dstAccelerationStructure = structure;
		geometryInfo.geometryCount = 1;
		geometryInfo.pGeometries = &geometry;
		geometryInfo.scratchData.deviceAddress = scratchBuffer.GetBufferAddress();

		buildRange.primitiveCount = 1;
		buildRange.primitiveOffset = 0;
		buildRange.firstVertex = 0;
		buildRange.transformOffset = 0;
	}
	void BLAS::Destroy()
	{
		scratchBuffer.Destroy();
		vertices.Destroy();
		indicies.Destroy();
		transform.Destroy();
	}
	void BLAS::Build(CommandBuffer* commandBuffer)
	{
		DEVICE_FUNCTION(device, vkCmdBuildAccelerationStructuresKHR);
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> buildRanges = { &buildRange };
		vkCmdBuildAccelerationStructuresKHR(commandBuffer->GetBuffer(), 1, &geometryInfo, buildRanges.data());
	}
	uint64_t BLAS::GetAddress()
	{
		DEVICE_FUNCTION(device, vkGetAccelerationStructureDeviceAddressKHR);
		VkAccelerationStructureDeviceAddressInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		info.accelerationStructure = structure;
		return vkGetAccelerationStructureDeviceAddressKHR(device->GetDevice(), &info);
	}
}

