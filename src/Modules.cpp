#include <simplistic/proc/Handle/Closer.h>
#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Modules.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>

#ifdef WINDOWS
#include <Windows.h>
#include <TlHelp32.h>
#endif

using namespace simplistic::proc;

ProcModules::ProcModules(std::uint64_t pid)
	: mPID(pid)
{}

#include <iostream>

#ifdef WINDOWS
// Generic module traverser
void TraverseModules(DWORD processID, std::function<bool(const ProcModuleInfo&)> callback) {
    // Create a snapshot of the modules in the process
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (hSnap == INVALID_HANDLE_VALUE) {
        throw ModuleSnapshotCreationFailedException();
    }

    HandleCloser hSnapCloser(hSnap); // Ensure proper resource cleanup

    MODULEENTRY32 moduleEntry{};
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    // Iterate over the modules in the snapshot
    if (Module32First(hSnap, &moduleEntry)) {
        do {
            if (!callback({
                reinterpret_cast<std::uint64_t>(moduleEntry.modBaseAddr),
                moduleEntry.modBaseSize,
                std::string(moduleEntry.szModule),
                })) break; // Report the module to the callback
        } while (Module32Next(hSnap, &moduleEntry));
    }
}
#elif defined(LINUX) || defined(ANDROID)
struct VMAreaStruct {
	std::uint64_t mStart;
	std::uint64_t mEnd;
	std::optional<std::string> mName;
};

void TraverseVmAreas(std::uint64_t pid, std::function<bool(const VMAreaStruct&)> cb) {
	std::ifstream mapsFile("/proc/" + std::to_string(pid) + "/maps");
	std::string line;
	while (std::getline(mapsFile, line)) {
		auto dashPos = line.find('-');
		auto spacePos = line.find(' ', dashPos);
		if (!cb({
			std::stoull(line.substr(0, dashPos), nullptr, 16),
			std::stoull(line.substr(dashPos + 1, spacePos - dashPos - 1), nullptr, 16),
			(spacePos = line.find_last_of(' ')) != std::string::npos && spacePos < line.size() - 1
				? std::optional<std::string>(line.substr(spacePos + 1))
				: std::nullopt
			})) break;
	}
}

void TraverseModulesRaw(std::uint64_t pid, std::function<bool(const simplistic::proc::ProcModuleInfo&)> cb)
{
	simplistic::proc::ProcModuleInfo currModule{};
	std::size_t segGaps = 0u;

	TraverseVmAreas(pid, [&](const VMAreaStruct& seg) {
		if (!seg.mName)
		{
			// At this point current segment has no name

			if (segGaps++ < 3 == false) return true;
			// At this point seg is a gap
			// lets update module
			currModule.mSize = seg.mEnd - seg.mStart;
			return true;
		}

		// At this point... segment has name

		if (!currModule.mName)
		{
			// At this point... the currModule start 
			// being in a valid state... so lets 
			// make seg the initial

			currModule.mName = *seg.mName;
			currModule.mBase = seg.mStart;
			currModule.mSize = seg.mEnd - seg.mStart;

			return true;
		}

		if (currModule.mName == *seg.mName)
		{
			// At this point... the name is 
			// same as the current segment
			// lets just extend the currModule
			// And move on
			segGaps = 0u;
			currModule.mSize = seg.mEnd - seg.mStart;
			return true;
		}

		// At this point... its time to report 
		// this End of module and setup
		// the beinning of this new one

		if (!cb(currModule)) return false;
		// At this point.. the user want more!!

		currModule.mBase = seg.mStart;
		currModule.mSize = seg.mEnd - seg.mStart;
		currModule.mName = *seg.mName;

		return true;
		});
}

void TraverseModules(std::uint64_t pid, std::function<bool(const simplistic::proc::ProcModuleInfo& mod)> cb)
{
	std::vector<simplistic::proc::ProcModuleInfo> all;
	TraverseModulesRaw(pid, [&](const simplistic::proc::ProcModuleInfo& mod) {
		all.push_back(mod);
		return true;
		});
	std::unordered_map<std::string, simplistic::proc::ProcModuleInfo> deduplicated;
	for (const auto& moduleInfo : all) {
		auto& entry = deduplicated[*moduleInfo.mName];
		if (entry.mSize >= moduleInfo.mSize)  continue;
		entry = moduleInfo;
	}
	for (const auto& [_, mod] : deduplicated)
		if (!cb(mod)) continue;
}
#endif

void ProcModules::ForEach(std::function<bool(const ProcModuleInfo& mod)> cb)
{
    TraverseModules(mPID, [&](const ProcModuleInfo& mod) {
        return cb(mod);
        });
}