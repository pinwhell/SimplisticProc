#pragma once

#include <simplistic/proc/IModules.h>

namespace simplistic {
	namespace proc {
		class ProcModules : public IProcModules {
		public:
			ProcModules(std::uint64_t pid);
			std::optional<ProcModuleInfo> GetInfo(std::string_view module, bool bStrict = true);

			std::uint64_t mPID;
		};
	}
}