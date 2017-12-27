#pragma once
#include "sc2api/sc2_api.h"

class CameraModule
{
private:
  sc2::Agent m_bot;
  std::vector<int> m_playerIDs;
  std::map<int,sc2::Point2D> m_startLocations;

public:
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

  CameraModule(sc2::Agent & bot);
  void onStart();
  void onFrame();
  const bool isNearOpponentStartLocation(sc2::Point2D pos, int player) const;
  const bool isNearOwnStartLocation(const sc2::Point2D pos, int player) const;
  const bool isArmyUnitType(sc2::UNIT_TYPEID type) const;
  const bool isBuilding(sc2::UNIT_TYPEID type) const;
  const bool isValidPos(const sc2::Point2D pos) const;
  const float Dist(const sc2::Unit * A, const sc2::Unit * B) const;
  const float Dist(const sc2::Point2D A, const sc2::Point2D B) const;
  void setPlayerStartLocations();
  void setPlayerIds();
  const int getOpponent(int player) const;
  void moveCamera(sc2::Point2D pos, int priority);
  void moveCamera(const sc2::Unit * unit, int priority);
  void moveCameraIsAttacking();
  void moveCameraIsUnderAttack();
  void moveCameraScoutWorker();
  void moveCameraFallingNuke();
  void moveCameraNukeDetect(sc2::Point2D target);
  void moveCameraDrop();
  const bool shouldMoveCamera(int priority) const;
  void moveCameraArmy();
  void moveCameraUnitCreated(const sc2::Unit * unit);
  void updateCameraPosition();
  const bool isUnderAttack(const sc2::Unit * unit) const;
  const bool isAttacking(const sc2::Unit * unit) const;
  const bool IsWorkerType(const sc2::UNIT_TYPEID type) const;
};
