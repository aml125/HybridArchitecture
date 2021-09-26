#pragma once
#include <string>
#include <vector>
#include <mutex>

namespace GM{
	struct Log {
		static void log(const std::string message);
		static void frameTime(double miliseconds);
		static void flush(const std::string& filename);
		static void flush();

	private:
		inline static std::vector<std::string> messages{};
		inline static std::vector<double> frameMeasures{};
		inline static std::mutex myLock;
		inline static std::mutex myLock2;
	};
}