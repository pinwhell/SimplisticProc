#pragma once

#include <simplistic/proc/Info.h>
#include <string_view>
#include <string>

namespace simplistic {
namespace proc {
	class MultiInfoBuilderFromName {
	public:
		MultiInfoBuilderFromName(std::string_view name = "", bool caseInsensitive = true);
		MultiInfoBuilderFromName& setName(const std::string& name);
		MultiInfoBuilderFromName& setCaseInsensitive(bool caseInsensitive);
		MultiInfo Build();

		std::string mName;
		bool mbCaseInsensitive;
	};

}
}