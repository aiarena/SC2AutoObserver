// ExampleObserver.cpp : Defines the entry point for the console application.
//
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

#include <iostream>
#include <windows.h>

#include "../../../CameraModule.h"
#include "Timer.hpp"


bool loadReplayPaths(const std::string & name, std::vector<std::string> & v)
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


class Replay : public sc2::ReplayObserver
{
public:
	CameraModuleObserver m_cameraModule;
	bool GameEnded;
	float speed;

	Replay(float speed) : sc2::ReplayObserver(),m_cameraModule(this),GameEnded(false),speed(speed)
	{
	}

	void OnGameStart() final
	{
		m_cameraModule.onStart();
	}

	void OnUnitCreated(const sc2::Unit* unit) final
	{
		m_cameraModule.moveCameraUnitCreated(unit);
	}

	void OnStep() final
	{
		Timer t;
		t.start();
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


int main(int argc, char* argv[]) {
	
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
		std::cout << "Please provide the path to a single SC2 replay or directory with replay files via --path." << std::endl;
		return 1;
	}
	float speed;
	std::string speedString;
	if (arg_parser.Get("Speed", speedString))
	{
		speed = atof(speedString.c_str());
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
	Replay replay_observer(speed);
	coordinator.AddReplayObserver(&replay_observer);
	while (true)
	{
		
		bool isDirectory = loadReplayPaths(replayPath, replayFiles);
		if (replayIndex == replayFiles.size())
		{
			std::cout << "There are no more replays at "<< replayPath <<"! I will wait for 30 seconds and try again.\n" << std::endl;
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
