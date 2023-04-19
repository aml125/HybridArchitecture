#include <game/util/timemeasure.hpp>
#include <game\util\log.hpp>
#include <sysinfoapi.h>

namespace GM {
	void TimeMeasure::StartCounter()
	{
		FILETIME fileTime{};
		ULARGE_INTEGER li;

		GetSystemTimePreciseAsFileTime(&fileTime);
		li.LowPart = fileTime.dwLowDateTime;
		li.HighPart = fileTime.dwHighDateTime;
		CounterStart = li.QuadPart;
	}

	double TimeMeasure::GetCounter()
	{
		FILETIME fileTime{};
		ULARGE_INTEGER li;
		GetSystemTimePreciseAsFileTime(&fileTime);
		li.LowPart = fileTime.dwLowDateTime;
		li.HighPart = fileTime.dwHighDateTime;
		return double(li.QuadPart - CounterStart) / 10000; // 100 nanosecond intervals to miliseconds
	}
}