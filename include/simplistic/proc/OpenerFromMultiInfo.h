#pragma once

#include <simplistic/proc/IProc.h>
#include <simplistic/proc/Info.h>
#include <vector>
#include <memory>

namespace simplistic {
namespace proc {
	using MultiProc = std::vector<std::unique_ptr<IProc>>;

	class ProcOpenerFromMultiInfo {
	public:
		ProcOpenerFromMultiInfo(const std::vector<Info>& multiInfo);
		std::unique_ptr<IProc> OpenOne();
		MultiProc OpenMany();
		std::vector<Info> mMultiInfo;
	};
}
}