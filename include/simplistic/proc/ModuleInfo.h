#pragma once

#include <string>
#include <cstdint>
#include <cstddef>
#include <optional>

namespace simplistic {
	namespace proc {
		struct ProcModuleInfo {
			std::uint64_t mBase;
			std::size_t mSize;
			std::optional<std::string> mName;
		};
	}
}