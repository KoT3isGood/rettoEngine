#pragma once
#include "..\LogicalDevice.h";
#include "..\CommandBuffer.h";
#include "..\Buffer.h";
#include "BLAS.h"

namespace rttvk {
	class TLAS : public Module {
	public:
		TLAS(LogicalDevice* device, BLAS* blas);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build(CommandBuffer* commandBuffer);
		VkAccelerationStructureKHR GetAS();
	private:
		LogicalDevice* device;
		BLAS* blas;
		Buffer blasInstance;
		Buffer tlasBuffer;
		Buffer scratchBuffer;

		VkAccelerationStructureKHR tlas;
		VkAccelerationStructureGeometryKHR geometryTLAS = {};
		VkAccelerationStructureBuildGeometryInfoKHR buildInfoTLAS = {};
		VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
	};
}
