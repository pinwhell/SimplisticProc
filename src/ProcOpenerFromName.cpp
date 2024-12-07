#include <simplistic/proc/ProcOpenerFromName.h>
#include <simplistic/proc/OpenerFromMultiInfo.h>
#include <simplistic/proc/Info/BuilderFromName.h>

using namespace simplistic::proc;

std::unique_ptr<IProc> ProcOpenerFromName::OpenOne(std::string_view name, bool caseInsensitive)
{
	return ProcOpenerFromMultiInfo(MultiInfoBuilderFromName(name, caseInsensitive)
		.Build())
		.OpenOne();
}

ProcOpenerFromName::Multi ProcOpenerFromName::OpenMany(std::string_view name, bool caseInsensitive)
{
	return ProcOpenerFromMultiInfo(MultiInfoBuilderFromName(name, caseInsensitive)
		.Build())
		.OpenMany();
}