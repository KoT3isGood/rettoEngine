#pragma once
#include "..\LogicalDevice.h";
#include "..\Buffer.h";
#include "..\CommandBuffer.h"
#include "Utils\Application\ProcessInfo.h"
#include "Utils\MeshLoader\MeshData.h"
namespace rttvk {
	class BLAS : public Module {
	public:
		BLAS() = default;
		BLAS(LogicalDevice* device, MeshData* meshData);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build();
		VkAccelerationStructureKHR* GetAccelerationStructure();
		VkDeviceAddress GetAccelerationStructureAddress();
		
		Buffer vertexBuffer;
		Buffer indexBuffer;
		Buffer uvBuffer;
		Buffer uvIndexesBuffer;
		Buffer materialIndexesBuffer;
		Buffer materialBuffer;
	private:
		MeshData* meshData;
		LogicalDevice* device;

		
		Buffer acBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
		VkAccelerationStructureGeometryKHR asGeom{};
		VkAccelerationStructureBuildRangeInfoKHR offset;
		VkAccelerationStructureBuildGeometryInfoKHR geometryInfo{};
		VkAccelerationStructureKHR blas;


		CommandPool cmdPool;
		CommandBuffer cmd;
	};
}

