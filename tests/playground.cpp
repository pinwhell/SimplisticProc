#include <simplistic/proc/Platform.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <simplistic/proc/ProcOpenerFromName.h>

using namespace simplistic::proc;

int main(int argc, const char** argv)
{
	try {
		std::filesystem::path selfExePath = argv[0];
		ProcOpenerFromName::Multi procInfos = ProcOpenerFromName()
			.OpenMany(selfExePath.filename().string());
		int x = 10;
		std::string example = "ThisIsAnAmazingExample - Lorem ipsum odor amet, consectetuer adipiscing elit. Nascetur taciti lacinia turpis rutrum ligula consectetur suscipit per magnis. Non aliquam litora parturient ullamcorper potenti pellentesque. Cras ipsum suspendisse nunc maecenas proin lobortis scelerisque urna. Ipsum gravida imperdiet cras; suscipit ex nascetur nulla praesent netus. Bibendum accumsan tempor sodales phasellus, dictum molestie tortor. Ante consectetur tortor bibendum nullam varius. Viverra turpis ornare at taciti mauris fames mus mus.";
		const char* pExample = example.data();
		//Self self{};
		auto& self = *procInfos[0];
		auto info = self.GetModules()
			->GetInfo(WIN_LNX("KERNEl32.DLL", "libc.so"), false);
		simplistic::io::Object k32Obj = *self.AddressModule(WIN_LNX("KERNEl32.DLL", "libc.so"), false);
		std::cout << std::boolalpha << (k32Obj.Read<std::uint16_t>() == 0x5A4Du) /* PE/DOS Magic */ << '\n';
		std::cout << info->mBase << std::endl;
		simplistic::io::Object obj = self.Address(0);
		auto y = obj.Read<int>(&x);
		std::string s1 = obj.ReadString<char>(example.data());
		std::string s2 = obj.DerrefString<char>(&pExample);
		auto s3 = obj
			.Address(&pExample)
			.Derref()
			.ReadString<char>();
		std::cout << std::boolalpha << (
			example == s1 &&
			example == s2 &&
			example == s3 &&
			obj.Derref(&pExample).mEntry == (std::uint64_t)pExample &&
			obj.Derref(&pExample).Read<char>() == 'T' &&
			*(std::uint32_t*)(obj.Derref(&pExample).ReadBlob(sizeof(std::uint32_t)).data()) == *(std::uint32_t*)example.data()
			);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}


