#pragma once

#include <simplistic/proc/IMultiOpenerFromName.h>
#include <simplistic/proc/IOneOpenerFromName.h>
#include <vector>
#include <memory>

namespace simplistic {
namespace proc {
	template<
		typename T,
		class TMulti = std::vector<std::unique_ptr<T>>,
		class TIMultiOpenerFromName = IMultiOpenerFromName<TMulti>,
		class TIOneOpenerFromName = IOneOpenerFromName<T>>
		class IOpenerFromName : public TIMultiOpenerFromName, public TIOneOpenerFromName {
		public:
			using Multi = TMulti;
			virtual ~IOpenerFromName() = default;
	};
}
}