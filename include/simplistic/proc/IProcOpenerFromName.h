#pragma once

#include <simplistic/proc/IOpenerFromName.h>
#include <simplistic/proc/IProc.h>

namespace simplistic {
namespace proc {
	using IProcOpenerFromName = IOpenerFromName<IProc>;
}
}