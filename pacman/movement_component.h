#pragma once
#include "component.h"
#include "visitable_map.h"
#include "map_visitor.h"

using namespace dir;

class MovementComponent : public Component, public MapVisitor
{
public:
	using MapVisitor::setVisitableMap;
	
	void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, float speed) {
		Component::Create(system, go, game_objects);
		this->speed = speed;
	}

	void setVisitableMap(VisitableMap * map) {
		this->map = map;
	}

	virtual void Update(float dt)
	{
		if (go->active)
		{
			Move(dt);

			if (!canContinue())
			{
				SDL_Log("Hit a wall (%d, %d)", (int)go->x, (int)go->y);
				go->dir = none;
			}
			setGameObjectDirection(getNextDirection());
		}
	}

	void Move(float dt) {
		const Translation * t;
		if (go->dir != none)
			go->pMovement += dt * speed;
		if (go->pMovement >= 1)
		{
			t = getTranslation(go->dir);
			Move(t->x, t->y);
			go->pMovement = 0;
			map->getTile(go->x, go->y)->visit();
		}
	}

	void setGameObjectDirection(Direction dir)
	{

		const Translation * t = getTranslation(dir);
		if (go->dir == none || go->pMovement == 0)
		{
			if (map->isAccessible(go->x + t->x, go->y + t->y)) //If GameObject can move in that direction, set new direction of game object.
				go->dir = dir;
		}
	}

	virtual Direction getNextDirection() = 0;

	void Move(float x, float y)
	{
		go->x += x;
		go->y += y;


		if (go->x > MAP_WIDTH - 1 && go->y != 13)
			go->x = MAP_WIDTH - 1;

		if (go->x > MAP_WIDTH - 1 && go->y == 13)
			go->x = 0;

		if (go->x < 0 && go->y != 13)
			go->x = 0;

		if (go->x < 0 && go->y == 13)
			go->x = MAP_WIDTH - 1;

		if (go->y > MAP_HEIGHT - 1)
			go->y = MAP_HEIGHT - 1;

		if (go->y < 0)
			go->y = 0;
	}

protected:
	VisitableMap * map;
private:
	inline bool canContinue() {
		const Translation * t = getTranslation(go->dir);
		return map->isAccessible(go->x + t->x, go->y + t->y);
	}
	float speed = 0;
};