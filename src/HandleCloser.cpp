#include <simplistic/proc/Utility/Handle.h>
#include <simplistic/proc/Handle/Closer.h>

#ifdef WINDOWS
#include<Windows.h>
#elif defined(LINUX)
#include <unistd.h>
#endif

using namespace simplistic::proc;

void simplistic::proc::HandleClose(Handle handle)
{
#ifdef WINDOWS
	CloseHandle(handle);
#elif defined(LINUX) || defined(ANDROID)
	close(handle);
#endif
}

HandleCloser::HandleCloser(Handle hProc, std::function<void(Handle)> closer)
	: mHandle(hProc)
	, mCloser(closer)
{}

HandleCloser::~HandleCloser()
{
	if (mCloser)
		mCloser(mHandle);
}