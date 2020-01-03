#include "CameraModule.h"
#include "Timer.hpp"
#include "Tools.h"

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_gametypes.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

#include <iostream>
#include <vector>


class Replay : public sc2::ReplayObserver
{
 public:
	CameraModuleObserver m_cameraModule;
	float m_speed;
	long m_delay;
	std::map < sc2::Tag, bool> alreadySeen;

	Replay(float speed, long delay):
		sc2::ReplayObserver(), m_cameraModule(this), m_speed(speed), m_delay(delay)
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

	void OnUnitEnterVision(const sc2::Unit*) final
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
		while (t.getElapsedTimeInMilliSec() < 1000.0 / 22.4 * static_cast<double>(m_speed))
		{
		}
	}

	void OnGameEnd() final
	{
		std::cout << "Game end." << std::endl;

		Timer t;
		t.start();
		while (t.getElapsedTimeInMilliSec() < m_delay)
		{
		}
	}
};


int main(int argc, char* argv[])
{
	sc2::ArgParser arg_parser(argv[0]);
	arg_parser.AddOptions({
		{ "-p", "--Path", "Path to a single SC2 replay or directory with replay files", true },
		{ "-s", "--Speed", "Replay speed", false},
		{ "-d", "--Delay", "Delay after game in ms.", false}
	});
	arg_parser.Parse(argc, argv);

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

	long delay;
	std::string delayString;
	if (arg_parser.Get("Delay", delayString))
	{
		delay = strtol(delayString.c_str(), nullptr, 10);
	}
	else
	{
		delay = 3000;
		std::cout << "Using default delay: 3000ms" << std::endl;
	}

	std::vector<std::string> replayFiles;
	unsigned long replayIndex = 0;
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}
	Replay replayObserver(speed, delay);
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
			sc2::SleepFor(30000);
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
