#include "ProcessInfo.h"

ProcessInfo engineProcessInfo {
};

ProcessInfo* getProcessInfo()
{
	return &engineProcessInfo;
}