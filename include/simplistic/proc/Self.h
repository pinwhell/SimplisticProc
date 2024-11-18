#pragma once

#include <simplistic/proc/IProc.h>
#include <simplistic/proc/Process.h>

namespace simplistic {
	namespace proc {
		class Self : public IProc {
		public:
			Self();
			std::size_t ReadRaw(const std::uint8_t* where, std::uint8_t* out, std::size_t len) override;
			std::size_t WriteRaw(std::uint8_t* where, const std::uint8_t* in, std::size_t len) override;
			bool Is64() override;
			static size_t Id();
			IProcModules* GetModules() override;

			Process mProc;
		};
	}
}