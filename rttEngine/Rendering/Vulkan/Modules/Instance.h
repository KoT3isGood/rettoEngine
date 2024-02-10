#pragma once
#include "Module.h"
#include <vector>

namespace rttvk {
	class Instance: Module {
	public:
		Instance(std::vector<const char*> extensions, std::vector<const char*> layers);
		virtual void Create() override;
		virtual void Destroy() override;
		VkInstance GetInstance();
	private:
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
		VkInstance instance;
	};
}
