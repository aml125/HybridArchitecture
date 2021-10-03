#include "Log.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace GM {
	void Log::log(const std::string message)
	{
		myLock.lock();
		messages.push_back(message);
		myLock.unlock();
		//std::cout << message << std::endl;
	}

	void Log::frameTime(double miliseconds)
	{
		myLock.lock();
		frameMeasures.push_back(miliseconds);
		myLock.unlock();
	}

	void Log::jayaTime(double miliseconds)
	{
		myLock.lock();
		jayaMeasures.push_back(miliseconds);
		myLock.unlock();
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

		//Timemeasures file
		myfile.open("fps_"+filename);
		double sum = 0;
		for (size_t i = 59; i < frameMeasures.size()-1; i++)
		{
			std::string avgStr = std::to_string(frameMeasures[i]);
			//std::replace(avgStr.begin(), avgStr.end(), '.', ',');
			myfile << avgStr << std::endl;
			sum += frameMeasures[i];
		}
		double average = sum / (frameMeasures.size() - 60 - 1);
		std::string avgStr = std::to_string(average);
		//std::replace(avgStr.begin(), avgStr.end(), '.', ',');
		myfile << "Average: " << avgStr << std::endl;
		myfile.close();

		//Jaya file
		myfile.open("ja_" + filename);
		sum = 0;
		for (size_t i = 1; i < jayaMeasures.size() - 1; i++)
		{
			std::string avgStr = std::to_string(jayaMeasures[i]);
			//std::replace(avgStr.begin(), avgStr.end(), '.', ',');
			myfile << avgStr << std::endl;
			sum += jayaMeasures[i];
		}
		average = sum / (jayaMeasures.size() - 2);
		avgStr = std::to_string(average);
		//std::replace(avgStr.begin(), avgStr.end(), '.', ',');
		myfile << "Average: " << avgStr << std::endl;
		myfile.close();
	}
	void Log::flush()
	{
		flush("Log.txt");
	}
}