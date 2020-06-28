#include "Tools.h"

#include <windows.h>


void pressDKey()
{
	auto hwnd = FindWindow(NULL, TEXT("StarCraft II"));
	if (hwnd != 0)
	{
		SetForegroundWindow(hwnd);
		SetFocus(hwnd);
		SetActiveWindow(hwnd);
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;  // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Press the "D" key
		ip.ki.wVk = 0x44;  // virtual-key code for the "D" key
		ip.ki.dwFlags = 0;  // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));

		// Release the "D" key.
		ip.ki.dwFlags = KEYEVENTF_KEYUP;  // KEYEVENTF_KEYUP for key release
		SendInput(1, &ip, sizeof(INPUT));
	}
}
