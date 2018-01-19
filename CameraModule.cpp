#include "sc2api/sc2_api.h"
#include "CameraModule.h"
#include "sc2api/sc2_proto_interface.h"
#include <iostream>

//Radius to detect groups of army units
const float armyBlobRadius = 10.0f;
//Movement speed of camera
const float moveFactor = 0.1f;
//Only smooth movemnt when closer as...
const float cameraJumpThreshold = 30.0f;
//When is a unit near a start location
const float nearStartLocationDistance = 50.0f;
//Camera distance to map (zoom). 0 means default.
const float cameraDistance = 50.0f;

CameraModule::CameraModule(sc2::Client * const bot):
	m_initialized(false),
	m_client(bot), 
	cameraMoveTime(200),
	cameraMoveTimeMin(75),
	watchScoutWorkerUntil(7500),
	lastMoved(0),
	lastMovedPriority(0),
	lastMovedPosition(sc2::Point2D(0, 0)),
	cameraFocusPosition(sc2::Point2D(0, 0)),
	cameraFocusUnit(nullptr),
	followUnit(false)
{
}

void CameraModule::onStart()
{
	if (m_client->Control() && m_client->Control()->GetObservation())
	{
		if (lastMoved)
		{
			lastMoved = 0;
			lastMovedPriority = 0;
			lastMovedPosition = sc2::Point2D(0, 0);
			cameraFocusUnit = nullptr;
			followUnit=false;
		}
		setPlayerIds();
		setPlayerStartLocations();
		cameraFocusPosition = (*m_startLocations.begin()).second;
		currentCameraPosition = (*m_startLocations.begin()).second;
		m_initialized = true;
	}
}

void CameraModule::onFrame()
{
	//Sometimes the first GetObservation() fails...
	if (!m_initialized)
	{
		onStart();
		return;
	}
	moveCameraFallingNuke();
	moveCameraNukeDetect();
	//moveCameraIsUnderAttack();
	moveCameraIsAttacking();
	if (m_client->Observation()->GetGameLoop() <= watchScoutWorkerUntil)
	{
		moveCameraScoutWorker();
	}
	moveCameraDrop();
	moveCameraArmy();

	updateCameraPosition();
}

void CameraModule::moveCameraFallingNuke()
{
	const int prio = 6;
	if (!shouldMoveCamera(prio))
	{
		return;
	}
	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if (unit->unit_type == sc2::UNIT_TYPEID::TERRAN_NUKE)
		{
			moveCamera(unit, prio);
			return;
		}
	}
}

void CameraModule::moveCameraNukeDetect()
{
	const int prio = 5;
	if (!shouldMoveCamera(prio))
	{
		return;
	}
	for (auto & effects : m_client->Observation()->GetEffects())
	{
		if (effects.effect_id == uint32_t(7)) //7 = NukePersistent NOT TESTED YET
		{
			moveCamera(effects.positions.front(), prio);
			return;
		}
	}
}

void CameraModule::moveCameraIsUnderAttack()
{
	const int prio = 4;
	if (!shouldMoveCamera(prio))
	{
		return;
	}

	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if (isUnderAttack(unit))
		{
			moveCamera(unit, prio);
		}
	}
}


void CameraModule::moveCameraIsAttacking()
{
	const int prio = 4;
	if (!shouldMoveCamera(prio))
	{
		return;
	}

	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if (isAttacking(unit))
		{
			moveCamera(unit, prio);
		}
	}
}

void CameraModule::moveCameraScoutWorker()
{
	const int highPrio = 2;
	const int lowPrio = 0;
	if (!shouldMoveCamera(lowPrio))
	{
		return;
	}

	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if (!IsWorkerType(unit->unit_type))
		{
			continue;
		}
		if (isNearOpponentStartLocation(unit->pos,unit->owner))
		{
			moveCamera(unit, highPrio);
		}
		else if (!isNearOwnStartLocation(unit->pos,unit->owner))
		{
			moveCamera(unit, lowPrio);
		}
	}
}

void CameraModule::moveCameraDrop()
{
	const int prio = 3;
	if (!shouldMoveCamera(prio))
	{
		return;
	}
	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if ((unit->unit_type.ToType() == sc2::UNIT_TYPEID::ZERG_OVERLORDTRANSPORT || unit->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_MEDIVAC || unit->unit_type.ToType() == sc2::UNIT_TYPEID::PROTOSS_WARPPRISM)
			&& isNearOpponentStartLocation(unit->pos,unit->owner) && unit->cargo_space_taken > 0)
		{
			moveCamera(unit, prio);
		}
	}
}

void CameraModule::moveCameraArmy()
{
	int prio = 1;
	if (!shouldMoveCamera(prio))
	{
		return;
	}
	// Double loop, check if army units are close to each other

	sc2::Point2D bestPos;
	const sc2::Unit * bestPosUnit = nullptr;
	int mostUnitsNearby = 0;

	for (auto & unit: m_client->Observation()->GetUnits())
	{
		if (!isArmyUnitType(unit->unit_type.ToType()) || unit->display_type!=sc2::Unit::DisplayType::Visible || unit->alliance==sc2::Unit::Alliance::Neutral)
		{
			continue;
		}
		sc2::Point2D uPos = unit->pos;

		int nrUnitsNearby = 0;
		for (auto & nearbyUnit : m_client->Observation()->GetUnits())
		{
			if (!isArmyUnitType(nearbyUnit->unit_type.ToType()) || unit->display_type != sc2::Unit::DisplayType::Visible || unit->alliance == sc2::Unit::Alliance::Neutral || Dist(unit->pos,nearbyUnit->pos)>armyBlobRadius)
			{
				continue;
			}
			nrUnitsNearby++;
		}
		if (nrUnitsNearby > mostUnitsNearby) {
			mostUnitsNearby = nrUnitsNearby;
			bestPos = uPos;
			bestPosUnit = unit;
		}
	}

	if (mostUnitsNearby > 1) {
		moveCamera(bestPosUnit, prio);
	}
}

void CameraModule::moveCameraUnitCreated(const sc2::Unit * unit)
{
	if (!m_initialized)
	{
		return;
	}
	int prio;
	if (isBuilding(unit->unit_type))
	{
		prio = 2;
	}
	else
	{
		prio = 1;
	}
	if (!shouldMoveCamera(prio) || unit->unit_type.ToType() == sc2::UNIT_TYPEID::TERRAN_KD8CHARGE)
	{
		return;
	}
	else if (!IsWorkerType(unit->unit_type))
	{
		moveCamera(unit, prio);
	}
}

const bool CameraModule::shouldMoveCamera(const int priority) const
{
	const int elapsedFrames = m_client->Observation()->GetGameLoop() - lastMoved;
	const bool isTimeToMove = elapsedFrames >= cameraMoveTime;
	const bool isTimeToMoveIfHigherPrio = elapsedFrames >= cameraMoveTimeMin;
	const bool isHigherPrio = lastMovedPriority < priority || (followUnit && !cameraFocusUnit->is_alive);
	// camera should move IF: enough time has passed OR (minimum time has passed AND new prio is higher)
	return isTimeToMove || (isHigherPrio && isTimeToMoveIfHigherPrio);
}

void CameraModule::moveCamera(const sc2::Point2D pos,const int priority)
{
	if (!shouldMoveCamera(priority))
	{
		return;
	}
	if (followUnit == false && cameraFocusPosition == pos)
	{
		// don't register a camera move if the position is the same
		return;
	}

	cameraFocusPosition = pos;
	lastMovedPosition = cameraFocusPosition;
	lastMoved = m_client->Observation()->GetGameLoop();
	lastMovedPriority = priority;
	followUnit = false;
}

void CameraModule::moveCamera(const sc2::Unit * unit, int priority)
{
	if (!shouldMoveCamera(priority))
	{
		return;
	}
	if (followUnit == true && cameraFocusUnit == unit) {
		// don't register a camera move if we follow the same unit
		return;
	}

	cameraFocusUnit = unit;
	lastMovedPosition = cameraFocusUnit->pos;
	lastMoved = m_client->Observation()->GetGameLoop();
	lastMovedPriority = priority;
	followUnit = true;
}

void CameraModule::updateCameraPosition()
{
	if (followUnit && isValidPos(cameraFocusUnit->pos))
	{
		cameraFocusPosition = cameraFocusUnit->pos;
	}
	//We only do smooth movement, if the focus is nearby.
	float dist = Dist(currentCameraPosition, cameraFocusPosition);
	if (dist > cameraJumpThreshold)
	{
		currentCameraPosition = cameraFocusPosition;
	}
	else if (dist > 0.1f)
	{
		currentCameraPosition = currentCameraPosition + sc2::Point2D(
			moveFactor*(cameraFocusPosition.x - currentCameraPosition.x),
			moveFactor*(cameraFocusPosition.y - currentCameraPosition.y));
	}
	else
	{
		return;
	}
	if (isValidPos(currentCameraPosition))
	{
		updateCameraPositionExcecute();
	}
}

//Utility

//At the moment there is no flag for being under attack
const bool CameraModule::isUnderAttack(const sc2::Unit * unit) const
{
	return false;
}

const bool CameraModule::isAttacking(const sc2::Unit * attacker) const
{
	if (!isArmyUnitType(attacker->unit_type.ToType()) || attacker->display_type != sc2::Unit::DisplayType::Visible || attacker->alliance == sc2::Unit::Alliance::Neutral)
	{
		return false;
	}
	//Option A
	//return unit->orders.size()>0 && unit->orders.front().ability_id.ToType() == sc2::ABILITY_ID::ATTACK_ATTACK;
	//Option B
	//return unit->weapon_cooldown > 0.0f;
	//Option C 
	//Not sure if observer can see the "private" fields of player units.
	//So we just assume: if it is in range and an army unit -> it attacks
	std::vector<sc2::Weapon> weapons = m_client->Observation()->GetUnitTypeData()[attacker->unit_type].weapons;
	float rangeAir = -1.0;
	float rangeGround = -1.0;
	for (auto & weapon : weapons)
	{
		if (weapon.type == sc2::Weapon::TargetType::Any)
		{
			rangeAir = weapon.range;
			rangeGround = weapon.range;
		}
		else if (weapon.type == sc2::Weapon::TargetType::Air)
		{
			rangeAir = weapon.range;
		}
		else if (weapon.type == sc2::Weapon::TargetType::Ground)
		{
			rangeGround = weapon.range;
		}
	}
	const int enemyID = getOpponent(attacker->owner);
	for (auto & unit : m_client->Observation()->GetUnits())
	{
		if (unit->display_type != sc2::Unit::DisplayType::Visible || unit->owner != enemyID || unit->alliance == sc2::Unit::Alliance::Neutral)
		{
			continue;
		}
		if (unit->is_flying)
		{
			if (Dist(attacker->pos, unit->pos) < rangeAir)
			{
				return true;
			}
		}
		else
		{
			if (Dist(attacker->pos, unit->pos) < rangeGround)
			{
				return true;
			}
		}
	}
	return false;
}

const bool CameraModule::IsWorkerType(const sc2::UNIT_TYPEID type) const
{
	switch (type)
	{
	case sc2::UNIT_TYPEID::TERRAN_SCV: return true;
	case sc2::UNIT_TYPEID::TERRAN_MULE: return true;
	case sc2::UNIT_TYPEID::PROTOSS_PROBE: return true;
	case sc2::UNIT_TYPEID::ZERG_DRONE: return true;
	case sc2::UNIT_TYPEID::ZERG_DRONEBURROWED: return true;
	default: return false;
	}
}

const bool CameraModule::isNearOpponentStartLocation(const sc2::Point2D pos,const int player) const
{
	return isNearOwnStartLocation(pos, getOpponent(player));
}

const bool CameraModule::isNearOwnStartLocation(const sc2::Point2D pos,const int player) const
{
	return Dist(pos, m_startLocations.at(player)) < nearStartLocationDistance;
}

const bool CameraModule::isArmyUnitType(const sc2::UNIT_TYPEID type) const
{
	if (IsWorkerType(type)) { return false; }
	if (type == sc2::UNIT_TYPEID::ZERG_OVERLORD) { return false; } //Excluded here the overlord transport etc to count them as army unit
	if (isBuilding(type)) { return false; }
	if (type == sc2::UNIT_TYPEID::ZERG_EGG) { return false; }
	if (type == sc2::UNIT_TYPEID::ZERG_LARVA) { return false; }

	return true;
}

const bool CameraModule::isBuilding(const sc2::UNIT_TYPEID type) const
{
	switch(type)
	{
		//Terran
	case sc2::UNIT_TYPEID::TERRAN_ARMORY:
	case sc2::UNIT_TYPEID::TERRAN_BARRACKS:
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING:
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
	case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
	case sc2::UNIT_TYPEID::TERRAN_BUNKER:
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
	case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
	case sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
	case sc2::UNIT_TYPEID::TERRAN_FACTORY:
	case sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING:
	case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
	case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
	case sc2::UNIT_TYPEID::TERRAN_FUSIONCORE:
	case sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY:
	case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET:
	case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
	case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
	case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
	case sc2::UNIT_TYPEID::TERRAN_REFINERY:
	case sc2::UNIT_TYPEID::TERRAN_SENSORTOWER:
	case sc2::UNIT_TYPEID::TERRAN_STARPORT:
	case sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING:
	case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
	case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
	case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
	case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
	case sc2::UNIT_TYPEID::TERRAN_REACTOR:
	case sc2::UNIT_TYPEID::TERRAN_TECHLAB:

		// Zerg
	case sc2::UNIT_TYPEID::ZERG_BANELINGNEST:
	case sc2::UNIT_TYPEID::ZERG_CREEPTUMOR:
	case sc2::UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:
	case sc2::UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:
	case sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
	case sc2::UNIT_TYPEID::ZERG_EXTRACTOR:
	case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:
	case sc2::UNIT_TYPEID::ZERG_HATCHERY:
	case sc2::UNIT_TYPEID::ZERG_HIVE:
	case sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN:
	case sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT:
	case sc2::UNIT_TYPEID::ZERG_LAIR:
	case sc2::UNIT_TYPEID::ZERG_LURKERDENMP:
	case sc2::UNIT_TYPEID::ZERG_NYDUSCANAL:
	case sc2::UNIT_TYPEID::ZERG_NYDUSNETWORK:
	case sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL:
	case sc2::UNIT_TYPEID::ZERG_SPINECRAWLER:
	case sc2::UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED:
	case sc2::UNIT_TYPEID::ZERG_SPIRE:
	case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER:
	case sc2::UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:
	case sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN:

		// Protoss
	case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR:
	case sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
	case sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE:
	case sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON:
	case sc2::UNIT_TYPEID::PROTOSS_FORGE:
	case sc2::UNIT_TYPEID::PROTOSS_GATEWAY:
	case sc2::UNIT_TYPEID::PROTOSS_NEXUS:
	case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON:
	case sc2::UNIT_TYPEID::PROTOSS_PYLON:
	case sc2::UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
	case sc2::UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
	case sc2::UNIT_TYPEID::PROTOSS_STARGATE:
	case sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
	case sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
	case sc2::UNIT_TYPEID::PROTOSS_WARPGATE:
		return true;
	}
	return false;
}

const bool CameraModule::isValidPos(const sc2::Point2D pos) const
{
	//Maybe playable width/height?
	return pos.x >= 0 && pos.y >= 0 && pos.x < m_client->Observation()->GetGameInfo().width && pos.y < m_client->Observation()->GetGameInfo().height;
}

const float CameraModule::Dist(const sc2::Unit * A, const sc2::Unit * B) const
{
	return std::sqrt(std::pow(A->pos.x - B->pos.x, 2) + std::pow(A->pos.y - B->pos.y, 2));
}

const float CameraModule::Dist(const sc2::Point2D A, const sc2::Point2D B) const
{
	return std::sqrt(std::pow(A.x - B.x, 2) + std::pow(A.y - B.y, 2));
}


void CameraModule::setPlayerStartLocations()
{
	m_startLocations.clear();
	std::vector<sc2::Point2D> startLocations = m_client->Observation()->GetGameInfo().start_locations;
	sc2::Units bases = m_client->Observation()->GetUnits(sc2::IsUnits({ sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER,sc2::UNIT_TYPEID::ZERG_HATCHERY,sc2::UNIT_TYPEID::PROTOSS_NEXUS }));
	// If we are not an observer
	// Assumes 2 player map
	if (bases.size() == 1)
	{
		m_startLocations[bases.front()->owner] = bases.front()->pos;
		m_startLocations[getOpponent(bases.front()->owner)] = getInvertedPos(bases.front()->pos);
	}
	else
	{
		for (auto & unit : bases)
		{
			m_startLocations[unit->owner] = unit->pos;
		}
	}
}

void CameraModule::setPlayerIds()
{
	for (auto & player : m_client->Observation()->GetGameInfo().player_info)
	{
		if (player.player_type != sc2::PlayerType::Observer)
		{
			m_playerIDs.push_back(player.player_id);
		}
	}
}

const int CameraModule::getOpponent(const int player) const
{
	for (auto & i : m_playerIDs)
	{
		if (i != player)
		{
			return i;
		}
	}
	return -1;
}

const sc2::Point2D CameraModule::getInvertedPos(const sc2::Point2D pos) const
{
	return sc2::Point2D(m_client->Observation()->GetGameInfo().width - pos.x, m_client->Observation()->GetGameInfo().width - pos.y);
}

CameraModule::~CameraModule()
{

}


///////////////////////// For observers
CameraModuleObserver::CameraModuleObserver(sc2::ReplayObserver * const observer):CameraModule(observer),m_observer(observer)
{

}

void CameraModuleObserver::onStart()
{
	//This should work once it is implemented on the proto side.
	sc2::GameRequestPtr request = m_observer->Control()->Proto().MakeRequest();
	SC2APIProtocol::RequestObserverAction* obsRequest = request->mutable_obs_action();
	SC2APIProtocol::ObserverAction* action = obsRequest->add_actions();
	SC2APIProtocol::ActionObserverPlayerPerspective * player_perspective = action->mutable_player_perspective();
	player_perspective->set_player_id(0); //0 = everyone
	m_client->Control()->Proto().SendRequest(request);
	m_client->Control()->WaitForResponse();
	CameraModule::onStart();
}

void CameraModuleObserver::updateCameraPositionExcecute()
{
	m_observer->ObserverAction()->CameraMove(currentCameraPosition,cameraDistance);
}





///////////////////////// For agents
CameraModuleAgent::CameraModuleAgent(sc2::Agent * const agent):CameraModule(agent)
{
	
}


void CameraModuleAgent::updateCameraPositionExcecute()
{
	sc2::GameRequestPtr camera_request = m_client->Control()->Proto().MakeRequest();
	SC2APIProtocol::RequestAction* request_action = camera_request->mutable_action();
	SC2APIProtocol::Action* action = request_action->add_actions();
	SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
	SC2APIProtocol::ActionRawCameraMove* camera_move = action_raw->mutable_camera_move();

	SC2APIProtocol::Point* point = camera_move->mutable_center_world_space();
	point->set_x(currentCameraPosition.x);
	point->set_y(currentCameraPosition.y);

	m_client->Control()->Proto().SendRequest(camera_request);
	m_client->Control()->WaitForResponse();
	//m_client->Debug()->DebugMoveCamera(currentCameraPosition);
	//m_client->Debug()->SendDebug();
}