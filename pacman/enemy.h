#pragma once
#include "movement_component.h"
#include "position_tracker.h"
#include "movable_position.h"
#include <list>

class Enemy : public GameObject
{

public:
	Enemy()
	{
		Coordinates::Coordinates(-1, -1);
	}

	virtual void Init(double xPos, double yPos)
	{
		SDL_Log("Alien::Init");
		this->x = xPos;
		this->y = yPos;

		dir = none;

		enabled = true;
		active = false;
	}
};

class EnemyBehaviourComponent : public MovementComponent, PositionTracker
{
public:
	virtual ~EnemyBehaviourComponent() {}

	void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects) {
		MovementComponent::Create(system, go, game_objects, ENEMY_SPEED);
	}

	virtual Direction getNextDirection()
	{
		switch (system->getEnemyState()) {
		case hunter:
			return hunterBehaviorDirection();
			break;
		case hunted:
			return huntedBehaviorDirection();
			break;
		}
	}

	virtual Direction hunterBehaviorDirection() = 0;

	virtual Direction huntedBehaviorDirection() = 0;

	using PositionTracker::trackPosition;
	void trackPosition(MovablePosition * track) {
		if (player)
			blinky = track;
		else
			player = track;
	}

protected:
	MovablePosition * blinky = nullptr;
	MovablePosition * player = nullptr;
	Coordinates lastPlayerPos = Coordinates(-1, -1);
	Coordinates lastGhostPos = Coordinates(-1, -1);
	Direction lastPlayerDirection = none;
	Direction lastGhostDirection = none;
	bool debug = false;

	inline Direction approachPlayerDir() {
		//We want to approach from ahead.
		Coordinates playerPos(*player);
		Coordinates ghostPos(*go);
		Direction playerDir = player->getDirection();
		if (playerPos == lastPlayerPos && ghostPos == lastGhostPos && lastPlayerDirection == playerDir)
			return lastGhostDirection;
		
		lastPlayerPos = playerPos;
		lastGhostPos = ghostPos;
		lastPlayerDirection = playerDir;
		list<Coordinates> ignore;
		const Translation * pT = getTranslation(playerDir);
		VisitableTile * playerNextITile = map->getNextIntersection(playerPos, player->dir);
		Coordinates playerNextIntersection(playerNextITile->getX(), playerNextITile->getY());
		Coordinates playerNext(player->x + pT->x, player->y + pT->y);
		Coordinates ghostTranslated1(go->x + pT->x, go->y + pT->y);
		Coordinates ghostTranslated2(go->x + 2 * pT->x, go->y + 2 * pT->y);
		
		if (playerDir != none && playerNext != ghostPos)
			ignore.push_back(playerPos);

		if ((playerPos != ghostTranslated1 && playerPos == ghostTranslated2))
			ignore.push_back(ghostTranslated1);

		if (playerPos == ghostTranslated1 || playerPos == ghostTranslated2)
			debug = true;

		switch (playerDir)
		{
		case left:
			ignore.push_back(*new Coordinates(player->getX() + 1, player->getY()));
			ignore.push_back(*new Coordinates(player->getX(), player->getY() + 1));
			ignore.push_back(*new Coordinates(player->getX(), player->getY() - 1));
			break;
		case right:
			ignore.push_back(*new Coordinates(player->getX() - 1, player->getY()));
			ignore.push_back(*new Coordinates(player->getX(), player->getY() + 1));
			ignore.push_back(*new Coordinates(player->getX(), player->getY() - 1));
			break;
		case up:
			ignore.push_back(*new Coordinates(player->getX(), player->getY() + 1));
			ignore.push_back(*new Coordinates(player->getX() + 1, player->getY()));
			ignore.push_back(*new Coordinates(player->getX() - 1, player->getY()));
			break;
		case down:
			ignore.push_back(*new Coordinates(player->getX(), player->getY() - 1));
			ignore.push_back(*new Coordinates(player->getX() + 1, player->getY()));
			ignore.push_back(*new Coordinates(player->getX() - 1, player->getY()));
			break;
		}

		if (playerPos == ghostPos) {
			lastGhostDirection = none;
			return lastGhostDirection;
		}
		else 
		{
			Path playerToIntersection = map->shortestPath(playerPos, playerNextIntersection, ignore);
			Path playerToGhost = map->shortestPath(playerPos, ghostPos, ignore);
			Path ghostToIntersection = map->shortestPath(ghostPos, playerNextIntersection, ignore);
			if (playerToGhost.length <= playerToIntersection.length) {
				lastGhostDirection = getOpposite(playerToGhost.path.back().dir);
				return lastGhostDirection;
			}
			else
			{
				lastGhostDirection = ghostToIntersection.path.front().dir;
				return lastGhostDirection;
			}
		}
	}

	inline Direction followPlayerDir(int maxProximity) {
		Coordinates playerPos(*player);
		Coordinates ghostPos(*go);
		Direction playerDir = player->getDirection();
		if (playerPos == lastPlayerPos && ghostPos == lastGhostPos && lastPlayerDirection == playerDir)
			return lastGhostDirection;

		lastPlayerPos = playerPos;
		lastGhostPos = ghostPos;
		lastPlayerDirection = playerDir;
		//We want to approach from ahead.

		list<Coordinates> ignore; //empty

		Path shortestPath = map->shortestPath(*go, *player, ignore);
		if (shortestPath.length < maxProximity)
		{
			Coordinates homecorner(12, 28);
			shortestPath = map->shortestPath(*go, homecorner, ignore);
		}

		if (shortestPath.length) // Not zero.
			return lastGhostDirection = (shortestPath.path.front().dir);
		else
			return lastGhostDirection = none;
	}
};

class PinkyBehaviourComponent : public EnemyBehaviourComponent {
public:
	virtual Direction hunterBehaviorDirection() {
		return approachPlayerDir();
	}

	virtual Direction huntedBehaviorDirection() {
		return none;
	}

};

class BlinkyBehaviourComponent : public EnemyBehaviourComponent {
public:
	virtual Direction hunterBehaviorDirection() {
		return followPlayerDir(0);
	}

	virtual Direction huntedBehaviorDirection() {
		return none;
	}

};

class InkyBehaviourComponent : public EnemyBehaviourComponent {
public:


	virtual Direction hunterBehaviorDirection() {
		list<Coordinates> ignore; //empty
		const Translation * bT = getTranslation(blinky->dir);
		const Translation * pT = getTranslation(player->dir);
		Coordinates b(blinky->x + bT->x , blinky->y + bT->y);
		Coordinates distance(player->x + pT->x, player->y + pT->y);
		Coordinates target(blinky->x, blinky->y);
		distance -= b;
		distance *= 2;
		target += distance;

		Path shortestPath = map->shortestPath(*go, target, ignore);

		if (shortestPath.length) // Not zero.
			return shortestPath.path.front().dir;
		else
			return none;
	}

	virtual Direction huntedBehaviorDirection() {
		//TODO
		return none;
	}

	int strategy = 0;
};

class ClydeBehaviourComponent : public EnemyBehaviourComponent {
public:
	virtual Direction hunterBehaviorDirection() {
		//TODO
		return followPlayerDir(8);
	}

	virtual Direction huntedBehaviorDirection() {
		//TODO
		return none;
	}

};