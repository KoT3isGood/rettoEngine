#pragma once
#include "..\LogicalDevice.h";
#include "..\Buffer.h";
#include "..\CommandBuffer.h";
namespace rttvk {
	class BLAS : public Module {
	public:
		BLAS(LogicalDevice* device);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build();
		VkAccelerationStructureKHR* GetAccelerationStructure();
		VkDeviceAddress GetAccelerationStructureAddress();
		
	private:
		LogicalDevice* device;

		Buffer vertexBuffer;
		Buffer indexBuffer;
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
