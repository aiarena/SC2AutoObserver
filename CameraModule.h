#pragma once
#include "sc2api/sc2_api.h"

class CameraModule
{
protected:

	//Sometimes getObservation fails when starting a game.
	bool m_initialized;

	sc2::Client * m_client;
	std::vector<int> m_playerIDs;
	std::map<int, sc2::Point2D> m_startLocations;


	int cameraMoveTime;
	int cameraMoveTimeMin;
	uint32_t watchScoutWorkerUntil;

	int lastMoved;
	int lastMovedPriority;
	sc2::Point2D lastMovedPosition;
	sc2::Point2D currentCameraPosition;
	sc2::Point2D cameraFocusPosition;
	const sc2::Unit *  cameraFocusUnit;
	bool followUnit;

	void moveCamera(const sc2::Point2D pos, const int priority);
	void moveCamera(const sc2::Unit * unit, int priority);
	void moveCameraIsAttacking();
	void moveCameraIsUnderAttack();
	void moveCameraScoutWorker();
	void moveCameraFallingNuke();
	void moveCameraNukeDetect();
	void moveCameraDrop();
	void moveCameraArmy();
	void updateCameraPosition();
	const bool shouldMoveCamera(const int priority) const;
	const bool isNearOpponentStartLocation(const sc2::Point2D pos, const int player) const;
	const bool isNearOwnStartLocation(const sc2::Point2D pos, int player) const;
	const bool isArmyUnitType(const sc2::UNIT_TYPEID type) const;
	const bool isBuilding(const sc2::UNIT_TYPEID type) const;
	const bool isValidPos(const sc2::Point2D pos) const;
	const bool isUnderAttack(const sc2::Unit * unit) const;
	const bool isAttacking(const sc2::Unit * unit) const;
	const bool IsWorkerType(const sc2::UNIT_TYPEID type) const;
	const float Dist(const sc2::Unit * A, const sc2::Unit * B) const;
	const float Dist(const sc2::Point2D A, const sc2::Point2D B) const;
	void setPlayerStartLocations();
	void setPlayerIds();
	const int getOpponent(const int player) const;
	const sc2::Point2D getInvertedPos(const sc2::Point2D) const;

	//Depending on whether we have an agent or an observer we need to use different functions to move the camera
	virtual void updateCameraPositionExcecute() = 0;

public:
	CameraModule(sc2::Client * const bot);
	void onStart();
	void onFrame();
	void moveCameraUnitCreated(const sc2::Unit * unit);

	~CameraModule();
};

class CameraModuleObserver : public CameraModule
{
private:
	sc2::ReplayObserver * const m_observer;
	void updateCameraPositionExcecute() override;
public:
	CameraModuleObserver(sc2::ReplayObserver * const observer);
	void onStart();
};

class CameraModuleAgent: public CameraModule
{
	void updateCameraPositionExcecute() override;
public:
	CameraModuleAgent(sc2::Agent * const observer);
};
