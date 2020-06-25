#include <game/util/timemeasure.hpp>
#include <game\util\log.hpp>

namespace GM {
	void TimeMeasure::StartCounter()
	{
		LARGE_INTEGER li;
		if (!QueryPerformanceFrequency(&li))
			GM::Log::log("QueryPerformanceFrequency failed!");

		PCFreq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	double TimeMeasure::GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - CounterStart) / PCFreq;
	}
}