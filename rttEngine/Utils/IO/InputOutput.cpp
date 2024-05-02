#include "InputOutput.h"




bool CheckIfButton(int key)
{
    SHORT output = GetKeyState(VK_LBUTTON);
    if (output == -127 || output == -128) {
        return 1;
    }
    return 0;
}

int* GetMousePosition()
{
    POINT point = {};
    if (GetCursorPos(&point)) {
        int output[2] = { point.x,point.y};
        return output;
    };
    return nullptr;
}

void SetMousePosition(int* position)
{
    SetCursorPos(position[0], position[1]);
}
