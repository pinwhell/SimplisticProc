#pragma once

#include <string_view>

namespace simplistic {
namespace proc {
	template<typename TMulti>
	class IMultiOpenerFromName {
	public:
		virtual ~IMultiOpenerFromName() = default;
		virtual TMulti OpenMany(std::string_view name, bool caseInsensitive = true) = 0;
	};
}
}