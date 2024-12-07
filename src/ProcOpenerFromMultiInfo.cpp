#include <simplistic/proc/OpenerFromMultiInfo.h>
#include <simplistic/proc/Process.h>
#include <algorithm>
#include <iterator>

using namespace simplistic::proc;

ProcOpenerFromMultiInfo::ProcOpenerFromMultiInfo(const std::vector<Info>& multiInfo)
	: mMultiInfo(multiInfo)
{}

std::unique_ptr<IProc> ProcOpenerFromMultiInfo::OpenOne()
{
	if (mMultiInfo.empty()) throw std::logic_error("Process Info list empty.");
	return std::make_unique<Process>(mMultiInfo[0].mPID);
}

MultiProc ProcOpenerFromMultiInfo::OpenMany()
{
	if (mMultiInfo.empty()) throw std::logic_error("Process Info list empty.");
	MultiProc multiProc{};
	std::transform(mMultiInfo.begin(), mMultiInfo.end(), std::back_inserter(multiProc), [](const Info& pi) {
		try {
			return std::make_unique<Process>(pi.mPID);
		}
		catch (const std::exception& e)
		{
			return std::unique_ptr<Process>{};
		}
		});
	multiProc.erase(std::remove_if(multiProc.begin(), multiProc.end(), [](const auto& proc) {
		return !proc;
		}), multiProc.end());
	return multiProc;
}