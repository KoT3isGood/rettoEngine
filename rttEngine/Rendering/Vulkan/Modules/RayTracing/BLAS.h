#pragma once
#include "..\LogicalDevice.h";
#include "..\CommandBuffer.h";
#include "..\Buffer.h";

namespace rttvk {
	class BLAS : public Module {
	public:
		BLAS(LogicalDevice* device, Buffer* buffer);
		virtual void Create() override;
		virtual void Destroy() override;
		void Build(CommandBuffer* commandBuffer);
		uint64_t GetAddress();
	private:
		LogicalDevice* device;
		Buffer* buffer;

		VkAccelerationStructureKHR structure;

		Buffer vertices;
		Buffer indicies;
		Buffer transform;
		Buffer scratchBuffer;

		VkAccelerationStructureBuildGeometryInfoKHR geometryInfo{};
		VkAccelerationStructureBuildRangeInfoKHR buildRange{};
		VkAccelerationStructureGeometryKHR geometry{};
		VkAccelerationStructureGeometryDataKHR geometryData{};
	};
}
