#pragma once

#include <memory>
#include <string_view>

namespace simplistic {
namespace proc {
	template<typename T>
	class IOneOpenerFromName {
	public:
		virtual ~IOneOpenerFromName() = default;
		virtual std::unique_ptr<T> OpenOne(std::string_view name, bool caseInsensitive = true) = 0;
	};
}
}