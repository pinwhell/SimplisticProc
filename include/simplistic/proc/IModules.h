#pragma once

#include <simplistic/proc/ModuleInfo.h>
#include <string_view>
#include <vector>
#include <optional>
#include <functional>

namespace simplistic {
	namespace proc {
		class IProcModules {
		public:
			virtual ~IProcModules() = default;
			virtual void ForEach(std::function<bool(const ProcModuleInfo& mod)> cb) = 0;
			virtual inline std::optional<ProcModuleInfo> GetInfo(std::string_view module, bool bStrict = true)
			{
				std::optional<ProcModuleInfo> res = std::nullopt;
				ForEach([&](const ProcModuleInfo& mod) {
					if (!mod.mName) return true;
					auto name = *mod.mName;
					bool matches = bStrict
						? (module == name)
						: (std::equal(name.begin(), name.end(), module.begin(), module.end(),
							[](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); }));
					if (!matches) return true;
					res = mod;
					return false;
					});
				return res;
			}
			virtual inline std::optional<std::vector<ProcModuleInfo>> GetAll() {
				std::vector<ProcModuleInfo> res;
				ForEach([&](const ProcModuleInfo& mod) {
					res.push_back(mod);
					return true;
					});
				return res;
			}
		};
	}
}