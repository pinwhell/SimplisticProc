#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Process.h>
#include <simplistic/proc/Platform.h>
#include <fstream>

#ifdef WINDOWS
#include<Windows.h>
#elif defined(LINUX)
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace simplistic::proc;

#ifdef WINDOWS
Process::Process(Handle handleOrFd)
	: mhProcess(handleOrFd)
	, mPid(GetProcessId(handleOrFd))
	, mModules(mPid)
{}
#endif

Process::Process(std::uint64_t pid)
	: mModules(pid)
{
	WIN_LNX(mHandleCloser, mMemHandleCloser) = std::make_unique<HandleCloser>([pid] {
		return WIN_LNX(
			OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid),
			(Handle)open((std::string("proc/") + std::to_string((int)pid) + "/mem").c_str(), O_RDWR)
		);
		}());
	if (WIN_LNX(
		mHandleCloser->mHandle == NULL,		/*Windows error*/
		(std::int64_t)mMemHandleCloser->mHandle < 0			/*Linux error*/
	)
		)
		throw ProcessOpenFailedException();
	WIN_LNX(mhProcess, mhMem) = WIN_LNX(mHandleCloser, mMemHandleCloser)->mHandle;
}

std::size_t Process::ReadRaw(const std::uint8_t* where, std::uint8_t* out, std::size_t len)
{
	WIN_LNX(SIZE_T, ssize_t) n {};

#ifdef WINDOWS
	if (ReadProcessMemory(mhProcess, where, out, len, &n) == FALSE && n == 0)
		return n;
#elif defined(LINUX)
	if ((n = pread64((int)mhMem, out, len, (off64_t)where)) < 0)
		return 0;
#endif
	return n;
}

std::size_t Process::WriteRaw(std::uint8_t* where, const std::uint8_t* in, std::size_t len)
{
	WIN_LNX(SIZE_T, ssize_t) n {};
#ifdef WINDOWS
	if (WriteProcessMemory(mhProcess, where, in, len, &n) == FALSE && n == 0)
		return n;
#elif defined(LINUX)
	if ((n = pwrite64((int)mhMem, in, len, (off64_t)where)) < 0)
		return 0;
#endif
	return n;
}

#ifdef LINUX
/*
Pseduo function to assess 32 or 64 bits 
based on address layouts found found in maps
*/
bool Is64(pid_t pid) {
	std::ifstream mapsFile("/proc/" + std::to_string(pid) + "/maps");
	if (!mapsFile.is_open()) return false;

	std::string line;
	while (std::getline(mapsFile, line)) {
		// Extract the address part before the '-'
		size_t pos = line.find('-');
		if (pos != std::string::npos && pos > 8) return true;
	}
	return false;
}
#endif

bool Process::Is64() {
#ifdef WINDOWS
	BOOL bIs32Bits = FALSE;
	return !IsWow64Process(mhProcess, &bIs32Bits) || !bIs32Bits;
#else
	return ::Is64(mPid);
#endif
}

IProcModules* Process::GetModules()
{
	return &mModules;
}