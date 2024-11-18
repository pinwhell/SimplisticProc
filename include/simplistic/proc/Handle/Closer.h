#pragma once

#include <simplistic/proc/Utility/Handle.h>
#include <functional>

namespace simplistic {
	namespace proc {
		class HandleCloser {
		public:
			HandleCloser(Handle hProc, std::function<void(Handle)> closer = HandleClose);
			~HandleCloser();

			std::function<void(Handle)> mCloser;
			Handle mHandle;
		};
	}
}