#pragma once
#include <string>
#include <functional>
#include <windows.h>

/*HERE WE ADD ALL THE FUNCTIONS WE WANT*/
void _RECORD() {
    INPUT inputs[4] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[2].type = INPUT_KEYBOARD;
    inputs[3].type = INPUT_KEYBOARD;

    inputs[0].ki.wVk = VK_MENU;
    inputs[1].ki.wVk = VK_F10;
    inputs[2].ki.wVk = VK_F10;
    inputs[3].ki.wVk = VK_MENU;

    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(4, inputs, sizeof(INPUT));
}

/*THE FUNCTION*/
struct Functions {
	std::string name;
	std::function<void()> function;
};

/*THE ARRAY OF FUNCTION TO SEND AND DO*/
Functions functionsArray[] = {
	Functions{std::string("Record"), _RECORD}
};

static const int SizeOfFunctionArray = sizeof(functionsArray) / sizeof(Functions);
