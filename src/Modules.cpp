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

#ifdef WINDOWS
// Generic module traverser
void TraverseModules(DWORD processID, const std::function<void(const MODULEENTRY32&)>& callback) {
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
            callback(moduleEntry); // Report the module to the callback
        } while (Module32Next(hSnap, &moduleEntry));
    }
}

std::optional<std::vector<ProcModuleInfo>> GetAllModules(DWORD processID) {
    std::vector<ProcModuleInfo> modules;

    try {
        TraverseModules(processID, [&](const MODULEENTRY32& moduleEntry) {
            modules.push_back({
                reinterpret_cast<std::uint64_t>(moduleEntry.modBaseAddr),
                static_cast<std::size_t>(moduleEntry.modBaseSize),
                std::string(moduleEntry.szModule)
                });
            });

        if (modules.empty()) {
            return std::nullopt; // Return nullopt if no modules are found
        }

        return modules; // Return the vector of modules
    }
    catch (...) {
        return std::nullopt; // Return nullopt in case of any errors
    }
}

// FindModuleInfo utility using the traverser
std::optional<ProcModuleInfo> FindModuleInfo(DWORD processID, std::string_view moduleName, bool caseSensitive = true) {
    // Function pointer for case-sensitive or case-insensitive comparison
    auto strcmp_fn = caseSensitive ? strcmp : _stricmp;
    std::optional<ProcModuleInfo> result;

    // Use TraverseModules to look for the module
    TraverseModules(processID, [&](const MODULEENTRY32& moduleEntry) {
        if (strcmp_fn(moduleEntry.szModule, moduleName.data()) == 0) {
            result = ProcModuleInfo{
                reinterpret_cast<std::uint64_t>(moduleEntry.modBaseAddr),
                static_cast<std::size_t>(moduleEntry.modBaseSize),
                std::string(moduleEntry.szModule)
            };
        }
        });

    return result; // Return the found module or std::nullopt
}
#endif

std::optional<ProcModuleInfo> ProcModules::GetInfo(std::string_view module, bool bStrict)
{
#ifdef WINDOWS
    return FindModuleInfo(mPID, module, bStrict);
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

std::optional<std::vector<ProcModuleInfo>> ProcModules::GetAll()
{
#ifdef WINDOWS
    return GetAllModules(mPID);
#else
    return std::nullopt;
#endif
}