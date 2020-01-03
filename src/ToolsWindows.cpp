#include "Tools.h"

#include <windows.h>


bool loadReplayPaths(std::string & name, std::vector<std::string> & v)
{
	if (name.find("2Replay") != std::string::npos)
	{
		v.push_back(name);
		return false;
	}

	std::string pattern(name);
	pattern.append("\\*");
	WIN32_FIND_DATAA data;
	HANDLE hFind;
	if ((hFind = FindFirstFileA(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (data.dwFileAttributes != 16lu)
			{
				const std::string fileName = name + "/" + data.cFileName;
				if (std::find(v.begin(), v.end(), fileName) == v.end())
				{
					v.push_back(fileName);
				}
			}
		}
		while (FindNextFileA(hFind, &data) != 0);
		FindClose(hFind);
	}
	return true;
}

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
