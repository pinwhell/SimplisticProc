#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Process.h>
#include <simplistic/proc/Platform.h>

#ifdef WINDOWS
#include<Windows.h>
#elif defined(LINUX)
#include <fcntl.h>
#endif

using namespace simplistic::proc;

Process::Process(Handle handleOrFd)
	: mhProcess(handleOrFd)
	, mModules(GetProcessId(handleOrFd))
{}

Process::Process(std::uint64_t pid)
	: mModules(pid)
{
	mHandleCloser = std::make_unique<HandleCloser>([pid] {
		return WIN_LNX(
			OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid),
			open(("proc/" + std::to_string((int)pid) + "/mem").c_str(), O_RDWR)
		);
		}());
	if (WIN_LNX(
		mHandleCloser->mHandle == NULL,		/*Windows error*/
		mHandleCloser->mHandle < 0			/*Linux error*/
	)
		)
		throw ProcessOpenFailedException();
	mhProcess = mHandleCloser->mHandle;
}

std::size_t Process::ReadRaw(const std::uint8_t* where, std::uint8_t* out, std::size_t len)
{
	WIN_LNX(SIZE_T, ssize_t) n {};

#ifdef WINDOWS
	if (ReadProcessMemory(mhProcess, where, out, len, &n) == FALSE && n == 0)
		throw simplistic::io::IOException();
#elif defined(LINUX)
	if ((n = pread(mhProcess, out, len, (off_t)where)) < 0)
		throw simplistic::io::IOException();
#endif
	return n;
}

std::size_t Process::WriteRaw(std::uint8_t* where, const std::uint8_t* in, std::size_t len)
{
	WIN_LNX(SIZE_T, ssize_t) n {};
#ifdef WINDOWS
	if (WriteProcessMemory(mhProcess, where, in, len, &n) == FALSE && n == 0)
		throw simplistic::io::IOException();
#elif defined(LINUX)
	if ((n = pwrite(mhProcess, in, len, (off_t)where)) < 0)
		throw simplistic::io::IOException();
#endif
	return n;
}

bool Process::Is64() {
	BOOL bIs32Bits = FALSE;
	return !IsWow64Process(mhProcess, &bIs32Bits) || !bIs32Bits;
}

IProcModules* Process::GetModules()
{
	return &mModules;
}