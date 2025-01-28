#include <simplistic/proc.h>
#include <iostream>

void TraverseModules(std::uint64_t pid, std::function<bool(const simplistic::proc::ProcModuleInfo&)> cb);

int main()
{
	TraverseModules(getpid(), [](const simplistic::proc::ProcModuleInfo& s) {
		std::cout << std::hex << s.mBase << " " << s.mSize << " " << (s.mName ? (*s.mName) + "\n" : "\n");
		return true;
		});
}