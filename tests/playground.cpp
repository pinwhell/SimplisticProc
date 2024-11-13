#include <simplistic/io.h>
#include <simplistic/proc/Platform.h>
#include <iostream>
#include <memory>
#ifdef WINDOWS
#include<Windows.h>
#elif defined(LINUX)
#include <unistd.h>
#endif
#include <functional>

class IProc : public simplistic::io::IIO {
public:
	virtual ~IProc() = default;
	virtual bool Is64() = 0;

	template<typename TAddr>
	inline simplistic::io::Object Address(TAddr where)
	{
		return simplistic::io::Object(this, where, Is64());
	}
};

using Handle = WIN_LNX(HANDLE, int);
void HandleClose(Handle handle)
{
#ifdef WINDOWS
	CloseHandle(handle);
#elif defined(LINUX) || defined(ANDROID)
	close(handle);
#endif
}

class HandleCloser {
public:
	HandleCloser(Handle hProc, std::function<void(Handle)> closer = HandleClose);
	~HandleCloser();

	std::function<void(Handle)> mCloser;
	Handle mHandle;
};

class ProcessOpenFailedException : public std::exception {
public:
	const char* what() const noexcept override;
};

class Process : public IProc {
public:
	Process(Handle handleOrFd);
	Process(std::uint64_t pid);
	std::size_t Read(const std::uint8_t* where, std::uint8_t* out, std::size_t len) override;
	std::size_t Write(std::uint8_t* where, const std::uint8_t* in, std::size_t len) override;
	bool Is64();

	std::unique_ptr<HandleCloser> mHandleCloser;
	Handle mhProcess;
};

class Self : public IProc {
public:
	Self();
	std::size_t Read(const std::uint8_t* where, std::uint8_t* out, std::size_t len) override;
	std::size_t Write(std::uint8_t* where, const std::uint8_t* in, std::size_t len) override;
	bool Is64() override;
	static size_t Id();

	Process mProc;
};

int main()
{
	int x = 10;
	std::string example = "ThisIsAnAmazingExample - Lorem ipsum odor amet, consectetuer adipiscing elit. Nascetur taciti lacinia turpis rutrum ligula consectetur suscipit per magnis. Non aliquam litora parturient ullamcorper potenti pellentesque. Cras ipsum suspendisse nunc maecenas proin lobortis scelerisque urna. Ipsum gravida imperdiet cras; suscipit ex nascetur nulla praesent netus. Bibendum accumsan tempor sodales phasellus, dictum molestie tortor. Ante consectetur tortor bibendum nullam varius. Viverra turpis ornare at taciti mauris fames mus mus.";
	const char* pExample = example.data();
	simplistic::io::Self io{};
	Self self{};
	simplistic::io::Object obj = self.Address(0);
	auto y = obj.Read<int>(&x);
	std::string s1 = obj.ReadString<char>(example.data());
	std::string s2 = obj.DerrefString<char>(&pExample);
	auto s3 = obj
		.Address(&pExample)
		.Derref(0)
		.ReadString<char>(0);
	std::cout << std::boolalpha << (
		example == s1 &&
		example == s2 &&
		example == s3 &&
		obj.Derref(&pExample).mEntry == (std::uint64_t)pExample &&
		obj.Derref(&pExample).Read<char>(0) == 'T' &&
		*(std::uint32_t*)(obj.Derref(&pExample).ReadBlob(0, 4).data()) == *(std::uint32_t*)example.data()
		);
	return 0;
}

inline HandleCloser::HandleCloser(Handle hProc, std::function<void(Handle)> closer)
	: mHandle(hProc)
	, mCloser(closer)
{}

HandleCloser::~HandleCloser()
{
	if (mCloser)
		mCloser(mHandle);
}

const char* ProcessOpenFailedException::what() const noexcept {
	return "Failed to open the process.";
}

Process::Process(Handle handleOrFd)
	: mhProcess(handleOrFd)
{}

Process::Process(std::uint64_t pid)
{
	mHandleCloser = std::make_unique<HandleCloser>([pid] {
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		}());
	if (mHandleCloser->mHandle == NULL)
		throw ProcessOpenFailedException();
	mhProcess = mHandleCloser->mHandle;
}

std::size_t Process::Read(const std::uint8_t* where, std::uint8_t* out, std::size_t len)
{
	SIZE_T n{};
	if (ReadProcessMemory(mhProcess, where, out, len, &n) == FALSE && n == 0)
		throw simplistic::io::IOException();
	return n;
}

std::size_t Process::Write(std::uint8_t* where, const std::uint8_t* in, std::size_t len)
{
	SIZE_T n{};
	if (WriteProcessMemory(mhProcess, where, in, len, &n) == FALSE && n == 0)
		throw simplistic::io::IOException();
	return n;
}

bool Process::Is64() {
	BOOL bIs32Bits = FALSE;
	return !IsWow64Process(mhProcess, &bIs32Bits) || !bIs32Bits;
}

// Inherited via IProc

Self::Self()
	: mProc(Id())
{}

std::size_t Self::Read(const std::uint8_t* where, std::uint8_t* out, std::size_t len)
{
	return mProc.Read(where, out, len);
}

std::size_t Self::Write(std::uint8_t* where, const std::uint8_t* in, std::size_t len)
{
	return mProc.Write(where, in, len);
}

bool Self::Is64()
{
	return mProc.Is64();
}

size_t Self::Id()
{
	return WIN_LNX(GetCurrentProcessId(), getpid());
}
