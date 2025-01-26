#pragma once

#include <simplistic/proc/ModuleInfo.h>
#include <string_view>
#include <vector>
#include <optional>

namespace simplistic {
	namespace proc {
		class IProcModules {
		public:
			virtual ~IProcModules() = default;
			virtual std::optional<ProcModuleInfo> GetInfo(std::string_view module, bool bStrict = true) = 0;
			virtual std::optional<std::vector<ProcModuleInfo>> GetAll() { return std::nullopt; }; // Unimpl by default
		};
	}
}