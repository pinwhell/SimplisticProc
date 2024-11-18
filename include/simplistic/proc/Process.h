#pragma once

#include <simplistic/proc/Handle/Closer.h>
#include <simplistic/proc/IProc.h>
#include <simplistic/proc/Handle.h>
#include <simplistic/proc/Modules.h>
#include <memory>

namespace simplistic {
	namespace proc {
		class Process : public IProc {
		public:
			Process(Handle handleOrFd);
			Process(std::uint64_t pid);
			std::size_t ReadRaw(const std::uint8_t* where, std::uint8_t* out, std::size_t len) override;
			std::size_t WriteRaw(std::uint8_t* where, const std::uint8_t* in, std::size_t len) override;
			bool Is64();
			IProcModules* GetModules() override;

			std::unique_ptr<HandleCloser> mHandleCloser;
			Handle mhProcess;
			ProcModules mModules;
		};
	}
}