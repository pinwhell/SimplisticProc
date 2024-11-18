#include <simplistic/proc/Handle/Closer.h>
#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Modules.h>

#ifdef WINDOWS
#include <Windows.h>
#include <TlHelp32.h>
#endif

using namespace simplistic::proc;

ProcModules::ProcModules(std::uint64_t pid)
	: mPID(pid)
{}

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
	throw std::logic_error("Method not implemented");
#endif
}
