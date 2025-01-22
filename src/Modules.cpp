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

#if defined(LINUX) || defined(ANDROID)
int _stricmp(const char* a, const char* b) {
	int ca, cb;
	do {
		ca = *(unsigned char*)a;
		cb = *(unsigned char*)b;
		ca = tolower(toupper(ca));
		cb = tolower(toupper(cb));
		a++;
		b++;
	} while (ca == cb && ca != '\0');
	return ca - cb;
}
#endif

#include <iostream>

std::optional<ProcModuleInfo> ProcModules::GetInfo(std::string_view module, bool bStrict)
{
	auto _strcmp = bStrict ? strcmp : _stricmp;
#ifdef WINDOWS
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, mPID);
	if (hSnap == INVALID_HANDLE_VALUE) throw ModuleSnapshotCreationFailedException();
	HandleCloser hSnapCloser(hSnap);
	MODULEENTRY32 curr{};
	curr.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &curr))
	{
		do {
			if (_strcmp(curr.szModule, module.data())) continue;
			return ProcModuleInfo{ (std::uint64_t)curr.modBaseAddr, (std::size_t)curr.modBaseSize };
		} while (Module32Next(hSnap, &curr));
	}
#ifndef SPROC_DISABLE_EXCEPTION
	throw ModuleNotFoundException();
#endif
	return {};
#else 
    // Open the /proc/[PID]/maps file to read memory mappings
    std::ifstream mapsFile("/proc/" + std::to_string(mPID) + "/maps");
    if (!mapsFile.is_open()) {
        throw std::runtime_error("Failed to open /proc/[PID]/maps for PID: " + std::to_string(mPID));
    }

    std::string line;
    bool foundModule = false;
    std::uint64_t startAddr = 0;
    std::uint64_t endAddr = 0;
    std::string currentModulePath;
    std::size_t moduleSize = 0;

    while (std::getline(mapsFile, line)) {
        // Parse the address range and permissions
        std::istringstream lineStream(line);
        std::string addrRange, perms, path;

        lineStream >> addrRange >> perms;  // We read address range and permissions
        if (addrRange.empty()) continue;    // Skip malformed lines

        // Skip non-readable segments (we only care about readable memory segments)
        if (perms[0] != 'r') {
            continue;
        }

        // Parse the address range (e.g., "04000000-04bcd000")
        size_t dashPos = addrRange.find('-');
        if (dashPos == std::string::npos) {
            continue; // Skip invalid lines without address range
        }

        std::string startStr = addrRange.substr(0, dashPos);
        std::uint64_t segmentStart = std::stoull(startStr, nullptr, 16); // Convert start address to uint64_t

        std::string endStr = addrRange.substr(dashPos + 1);
        std::uint64_t segmentEnd = std::stoull(endStr, nullptr, 16); // Convert end address to uint64_t
        // Parse the module path (last part of the line)
        while (lineStream >> path);  // The last element should be the file path

        // Check if the path contains the module name
        if (strstr(path.c_str(), module.data())) {
            if (!foundModule) {
                // If we find the first occurrence, set the base address and size
                foundModule = true;
                startAddr = segmentStart;
                currentModulePath = path;
                endAddr = segmentEnd;
                moduleSize = endAddr - startAddr;
            }
            else if (path == currentModulePath) {
                // If it's the same module and segment, accumulate the size
                moduleSize += segmentEnd - endAddr;
                endAddr = segmentEnd; // Update the end address
            }
        }
        else if (foundModule) break;
    }

    if (!foundModule) {
        return {}; // Return empty if module is not found
    }

    return ProcModuleInfo{ startAddr, moduleSize };
#endif
}
