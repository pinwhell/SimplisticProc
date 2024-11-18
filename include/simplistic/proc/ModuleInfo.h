#pragma once

#include <cstdint>
#include <cstddef>

namespace simplistic {
	namespace proc {
		struct ProcModuleInfo {
			std::uint64_t mBase;
			std::size_t mSize;
		};
	}
}