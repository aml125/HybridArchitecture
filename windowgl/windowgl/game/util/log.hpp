#pragma once
#include <string>
#include <vector>

namespace GM{
	struct Log {
		static void log(const std::string& message);
		static void flush(const std::string& filename);
		static void flush();

	private:
		inline static std::vector<std::string> messages{};
	};
}