#pragma once
#include <windows.h>
#include <iostream>
namespace GM {
	struct TimeMeasure {
	static void StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			std::cout << "QueryPerformanceFrequency failed!\n";

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	static double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}

	private:
		inline static double PCFreq = 0.0;
		inline static __int64 CounterStart = 0;
	};
}