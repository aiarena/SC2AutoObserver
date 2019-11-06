// ExampleObserver.cpp : Defines the entry point for the console application.
//
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

#include <iostream>
#include <windows.h>
#include <vector>

#include "../../../CameraModule.h"
#include "Timer.hpp"
#include "sc2api/sc2_gametypes.h"

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

void pressAltEnter()
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

		// Press keys
		ip.ki.dwFlags = 0;  // 0 for key press

		// Press the "alt" key
		ip.ki.wVk = 0x12;  // virtual-key code for the "alt" key
		SendInput(1, &ip, sizeof(INPUT));

		// Press the "enter" key
		ip.ki.wVk = 0x0D;  // virtual-key code for the "alt" key
		SendInput(1, &ip, sizeof(INPUT));

		// Release keys
		ip.ki.dwFlags = KEYEVENTF_KEYUP;  // KEYEVENTF_KEYUP for key release

		// Release the "alt" key
		ip.ki.wVk = 0x12;
		SendInput(1, &ip, sizeof(INPUT));

		// Release the "enter" key
		ip.ki.wVk = 0x0D;
		SendInput(1, &ip, sizeof(INPUT));
	}
}

class Replay : public sc2::ReplayObserver
{
 public:
	CameraModuleObserver m_cameraModule;
	bool GameEnded;
	float speed;
	std::map < sc2::Tag, bool> alreadySeen;

	Replay(float speed) : sc2::ReplayObserver(), m_cameraModule(this), GameEnded(false), speed(speed)
	{
	}

	void OnGameStart() final
	{
		/* For some reason it is not a good idea to already do this on start.
		for (const auto & unit : Observation()->GetUnits())
		{
			alreadySeen[unit->tag] = true;
		}
		*/
		m_cameraModule.onStart();
	}

	void OnUnitCreated(const sc2::Unit* unit) final
	{
		m_cameraModule.moveCameraUnitCreated(unit);
	}

	void OnUnitEnterVision(const sc2::Unit* unit) final
	{
		// Does not get called.
	}

	void OnStep() final
	{
		if (Observation()->GetGameLoop() == 10)
		{
			pressDKey();
		}
		Timer t;
		t.start();
		Observation()->GetChatMessages();		
		for (const auto & unit : Observation()->GetUnits())
		{
			if (!alreadySeen[unit->tag])
			{
				alreadySeen[unit->tag] = true;
				OnUnitCreated(unit);
			}
		}
		m_cameraModule.onFrame();
		while (t.getElapsedTimeInMilliSec() < 1000.0 / (22.4*speed))
		{
		}
	}

	void OnGameEnd() final
	{
		GameEnded = true;
		std::cout << "Game end." << std::endl;
	}
};


int main(int argc, char* argv[])
{
	sc2::ArgParser arg_parser(argv[0]);
	arg_parser.AddOptions({
		{ "-p", "--Path", "Path to a single SC2 replay or directory with replay files", true },
		{ "-s", "--Speed", "Replay speed", false}
	});
	arg_parser.Parse(argc, argv);
	std::string GamePortStr;

	std::string replayPath;
	if (!arg_parser.Get("Path", replayPath))
	{
		std::cout << "Please provide the path to a single SC2 replay or directory with replay files via --Path." << std::endl;
		return 1;		
	}
	float speed;
	std::string speedString;
	if (arg_parser.Get("Speed", speedString))
	{
		speed = strtof(speedString.c_str(), nullptr);
	}
	else
	{
		speed = 4.0f;
		std::cout << "Using default speed: 4x" << std::endl;
	}
	std::vector<std::string> replayFiles;
	int replayIndex = 0;
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}
	Replay replayObserver(speed);
	coordinator.AddReplayObserver(&replayObserver);
	coordinator.SetReplayPerspective(0);
	//coordinator.SetRealtime(true);
	coordinator.SetMultithreaded(true);
	coordinator.SetFullScreen(1);	
	while (true)
	{
		bool isDirectory = loadReplayPaths(replayPath, replayFiles);
		if (replayIndex == replayFiles.size())
		{
			std::cout << "There are no more replays at " << replayPath << "\\*" << std::endl << "I will wait for 30 seconds and try again." << std::endl;
			if (replayIndex == 0)
			{
				std::cout << "If you provide a directory please do not end the path with an '\'" << std::endl;
			}
			Sleep(30000);
			continue;
		}
		const std::string replayFile = replayFiles[replayIndex];
		if (!coordinator.SetReplayPath(replayFile))
		{
			std::cout << "Could not read the replay: " << replayFile << std::endl;
			std::cout << "Please provide the replay path as command line argument." << std::endl;
			return 1;
		}
		if (!coordinator.HasReplays())
		{
			std::cout << "Could not read the replay: " << replayFile << std::endl;
			std::cout << "Please provide the replay path as command line argument." << std::endl;
			return 1;
		}
		if (!FindWindow(NULL, TEXT("StarCraft II")))
		{
			// Update once so that the game launches
			coordinator.Update();
			// Wait for launch and then Full screen mode
			int counter = 0;
			while (!FindWindow(NULL, TEXT("StarCraft II")) && counter < 10)
			{
				++counter;
				sc2::SleepFor(500);
			}
			//if (counter < 10)
			//{
			//	pressAltEnter();
			//}
		}
		while (coordinator.Update() && !coordinator.AllGamesEnded())
		{
		}
		if (!isDirectory)
		{
			break;
		}
		++replayIndex;
	}
	coordinator.LeaveGame();
}
