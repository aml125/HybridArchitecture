#pragma once
#include <windows.h>
#include <iostream>
#include <Profileapi.h>

namespace GM {
	struct TimeMeasure { // TODO CAMBIAR A https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtimepreciseasfiletime
		void StartCounter();
		double GetCounter();

	private:
		double PCFreq = 0.0;
		__int64 CounterStart = 0;
	};
}