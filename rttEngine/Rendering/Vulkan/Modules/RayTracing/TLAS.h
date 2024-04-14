#pragma once
#include "..\LogicalDevice.h";
#include "..\Buffer.h"
#include "..\CommandBuffer.h";
#include "BLAS.h";
namespace rttvk {
	class TLAS : public Module {
	public:
		TLAS(LogicalDevice* device, BLAS* blas);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build();
		VkAccelerationStructureKHR* GetAccelerationStructure();
		
		
	private:
		BLAS* blas;

		LogicalDevice* device;

		Buffer instanceBuffer;
		Buffer acBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureInstanceKHR instance{};
		VkAccelerationStructureGeometryInstancesDataKHR instances{};
		VkAccelerationStructureGeometryKHR asGeom{};
		VkAccelerationStructureBuildRangeInfoKHR offset;
		VkAccelerationStructureBuildGeometryInfoKHR geometryInfo{};
		VkAccelerationStructureKHR tlas;

		CommandPool cmdPool;
		CommandBuffer cmd;
	};
}
