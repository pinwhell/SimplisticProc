#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace simplistic {
namespace proc {
	struct Info {
		std::uint64_t mPID;
		std::string mName;
	};

	using MultiInfo = std::vector<Info>;
}
}