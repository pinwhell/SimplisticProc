#pragma once

#include <simplistic/proc/IProcOpenerFromName.h>

namespace simplistic {
namespace proc {
	class ProcOpenerFromName : public IProcOpenerFromName {
	public:
		std::unique_ptr<IProc> OpenOne(std::string_view name, bool caseInsensitive = true);
		Multi OpenMany(std::string_view name, bool caseInsensitive = true);
	};
}
}