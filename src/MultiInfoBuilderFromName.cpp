#include <simplistic/proc/Info/BuilderFromName.h>
#include <simplistic/proc/Exceptions.h>
#include <simplistic/proc/Handle/Closer.h>
#include <Windows.h>
#include <TlHelp32.h>

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
	throw std::logic_error("Method not implemented");
#endif
}