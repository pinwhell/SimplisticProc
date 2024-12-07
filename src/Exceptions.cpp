#include <simplistic/proc/Exceptions.h>

using namespace simplistic::proc;

const char* ProcessOpenFailedException::what() const noexcept {
	return "Failed to open the process.";
}

const char* ProcessSnapshotCreationFailedException::what() const noexcept {
	return "Failed to create process snapshot.";
}

const char* ModuleSnapshotCreationFailedException::what() const noexcept {
	return "Failed to create module snapshot.";
}

const char* ModuleNotFoundException::what() const noexcept {
	return "Module not found.";
}