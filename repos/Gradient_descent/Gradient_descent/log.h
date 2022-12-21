#pragma once
#include <string>
#include <vector>
#include <mutex>

namespace GM {
	void log(const std::string message);
	void frameTime(double miliseconds);
	void jayaTime(double miliseconds);
	void flush(const std::string& filename);
	void flush();

	static std::vector<std::string> messages{};
	static std::vector<double> frameMeasures{};
	static std::vector<double> jayaMeasures{};
	static std::mutex myLock;
	static std::mutex myLock2;
}