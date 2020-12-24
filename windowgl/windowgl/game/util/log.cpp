#include "Log.hpp"
#include <fstream>
#include <iostream>

namespace GM {
	void Log::log(const std::string message)
	{
		myLock.lock();
		messages.push_back(message);
		myLock.unlock();
		//std::cout << message << std::endl;
	}
	void Log::flush(const std::string& filename)
	{
		log("Writting logs to a file");
		std::ofstream myfile;
		myfile.open(filename);
		for (size_t i = 0; i < messages.size(); i++)
		{
			myfile << "[" << std::to_string(i) << "] " << messages[i] << std::endl;
		}
		myfile.close();
	}
	void Log::flush()
	{
		flush("Log.txt");
	}
}