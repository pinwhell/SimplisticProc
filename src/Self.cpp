#include <simplistic/proc/Self.h>
#include <simplistic/proc/Platform.h>

#ifdef WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace simplistic::proc;

Self::Self()
	: mProc(Id())
{}

std::size_t Self::ReadRaw(const std::uint8_t* where, std::uint8_t* out, std::size_t len)
{
	return mProc.ReadRaw(where, out, len);
}

std::size_t Self::WriteRaw(std::uint8_t* where, const std::uint8_t* in, std::size_t len)
{
	return mProc.WriteRaw(where, in, len);
}

bool Self::Is64()
{
	return mProc.Is64();
}

size_t Self::Id()
{
	return WIN_LNX(GetCurrentProcessId(), getpid());
}

IProcModules* Self::GetModules()
{
	return mProc
		.GetModules();
}