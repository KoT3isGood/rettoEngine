#include "ProcessInfo.h"

ProcessInfo engineProcessInfo {
};

ProcessInfo* GetProcessInfo()
{
	return &engineProcessInfo;
}
// Required in rendering
auto AppStart = std::chrono::high_resolution_clock::now();



float getRunningTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	return float(std::chrono::duration_cast<std::chrono::microseconds>(currentTime- AppStart).count())*0.000001;
}