// ExampleObserver.cpp : Defines the entry point for the console application.
//
#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

#include <iostream>

#include "../../../CameraModule.h"

class Replay : public sc2::ReplayObserver
{
public:
	CameraModuleObserver m_cameraModule;

	Replay() : sc2::ReplayObserver(),m_cameraModule(*this)
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
		m_cameraModule.onFrame();
	}

	void OnGameEnd() final
	{
		std::cout << "Game end." << std::endl;
	}
};


int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		std::cout << "Please provide the replay path as command line argument." << std::endl;
		return 1;
	}
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc-1, argv)) {
		return 1;
	}
	const char* replayFile = argv[argc - 1];
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

	Replay replay_observer;
	coordinator.AddReplayObserver(&replay_observer);
	while (coordinator.Update() && !coordinator.AllGamesEnded())
	{

	}
	coordinator.LeaveGame();
}
