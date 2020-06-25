#pragma once
#include <windows.h>
#include <iostream>
namespace GM {
	struct TimeMeasure {
		void StartCounter();
		double GetCounter();

	private:
		double PCFreq = 0.0;
		__int64 CounterStart = 0;
	};
}