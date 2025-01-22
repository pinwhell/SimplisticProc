#include <simplistic/proc/Info/BuilderFromName.h>
#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Handle/Closer.h>
#include <simplistic/proc/Platform.h>
#include <fstream>
#ifdef WINDOWS
#include <Windows.h>
#include <TlHelp32.h>
#elif defined(LINUX)
#include <dirent.h>
#include <unistd.h>
#endif

using namespace simplistic::proc;

MultiInfoBuilderFromName::MultiInfoBuilderFromName(std::string_view name, bool caseInsensitive)
	: mName(name)
	, mbCaseInsensitive(caseInsensitive)
{}

MultiInfoBuilderFromName& MultiInfoBuilderFromName::setName(const std::string& name)
{
	mName = name;
	return *this;
}

MultiInfoBuilderFromName& MultiInfoBuilderFromName::setCaseInsensitive(bool caseInsensitive)
{
	mbCaseInsensitive = caseInsensitive;
	return *this;
}

#if defined(LINUX) || defined(ANDROID)
extern int _stricmp(const char* a, const char* b); // Defined elsewhere
#endif

MultiInfo MultiInfoBuilderFromName::Build()
{
	MultiInfo res{};
	auto _strcmp = mbCaseInsensitive ? _stricmp : strcmp;
#ifdef WINDOWS
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnap == INVALID_HANDLE_VALUE) throw ProcessSnapshotCreationFailedException();
	HandleCloser hSnapCloser(hSnap);
	PROCESSENTRY32 curr{};
	curr.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnap, &curr))
	{
		do {
			if (_strcmp(curr.szExeFile, mName.data())) continue;
			res.emplace_back(Info{ curr.th32ProcessID, curr.szExeFile });
		} while (Process32Next(hSnap, &curr));
	}
	return res;
#else 
    // Open the /proc directory to scan all processes
    DIR* procDir = opendir("/proc");
    if (!procDir) {
        throw std::runtime_error("Failed to open /proc directory");
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        // Skip entries that are not directories or not numeric (i.e., not PIDs)
        if (entry->d_type != DT_DIR || !isdigit(entry->d_name[0])) {
            continue;
        }

        std::uint64_t pid = std::stoi(entry->d_name); // Process ID (PID)

        // Read the /proc/[pid]/comm file directly to get the executable name
        std::string commPath = "/proc/" + std::string(entry->d_name) + "/comm";
        std::ifstream commFile(commPath);
        if (!commFile.is_open()) {
            continue; // Skip if we can't read the comm file
        }

        std::string exeName;
        std::getline(commFile, exeName); // Read the executable name (one line)
        commFile.close();

        // Compare the executable name with mName
        if (exeName.find(mName) != std::string::npos) {
            res.emplace_back(Info{ pid, exeName }); // Add process to result list
        }
    }

    closedir(procDir); // Close /proc directory
    return res;
#endif
}