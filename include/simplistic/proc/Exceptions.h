#pragma once

#include <stdexcept>

namespace simplistic {
	namespace proc {
		class ProcessOpenFailedException : public std::exception {
		public:
			const char* what() const noexcept override;
		};

		class ModuleSnapshotCreationFailedException : public std::exception {
		public:
			const char* what() const noexcept override;
		};

		class ModuleNotFoundException : public std::exception {
		public:
			const char* what() const noexcept override;
		};
	}
}