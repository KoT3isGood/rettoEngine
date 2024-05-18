#pragma once

#include "Utils\Application\ProcessInfo.h"

#ifdef Win64
#include <windows.h>
#endif

bool IsKeyDown(int key);
int* GetMousePosition();
void SetMousePosition(int* position);

bool IsGameActive();