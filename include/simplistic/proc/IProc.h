#pragma once

#include <simplistic/proc/IModules.h>
#include <simplistic/io/IIO.h>
#include <simplistic/io/Object.h>

namespace simplistic {
	namespace proc {
		class IProc : public simplistic::io::IIO {
		public:
			virtual ~IProc() = default;
			virtual bool Is64() = 0;
			virtual IProcModules* GetModules() = 0;

			template<typename TAddr>
			inline simplistic::io::Object Address(TAddr where)
			{
				return simplistic::io::Object(this, where, Is64());
			}
		};
	}
}