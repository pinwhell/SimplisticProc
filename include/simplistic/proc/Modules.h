#pragma once

#include <simplistic/proc/IModules.h>

namespace simplistic {
	namespace proc {
		class ProcModules : public IProcModules {
		public:
			ProcModules(std::uint64_t pid);
			void ForEach(std::function<bool(const ProcModuleInfo& mod)> cb);
			std::uint64_t mPID;
		};
	}
}