#pragma once
#include "sc2api/sc2_api.h"

class CameraModule
{
private:
  sc2::Point2D myStartLocation;
  sc2::Agent m_bot;

public:
  int scrWidth;
  int scrHeight;
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
  void onStart(int screenWidth=0, int screenHeight=0);
  void onFrame();
  bool isNearEnemyStartLocation(sc2::Point2D pos);
  const bool isNearOwnStartLocation(const sc2::Point2D pos) const;
  const bool isArmyUnitType(sc2::UNIT_TYPEID type) const;
  const bool isBuilding(sc2::UNIT_TYPEID type) const;
  const bool isValidPos(const sc2::Point2D pos) const;
  const float Dist(const sc2::Unit * A, const sc2::Unit * B) const;
  const float Dist(const sc2::Point2D A, const sc2::Point2D B) const;
  const sc2::Point2DI ConvertWorldToMinimap(const sc2::Point2D & world) const;
  const sc2::Point2D getPlayerStartLocation() const;
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
  bool isNearEnemyBuilding(const sc2::Unit * unit, sc2::Units enemyUnits) const;
};
