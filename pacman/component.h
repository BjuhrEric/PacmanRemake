#pragma once
#include "object_pool.h"
#include <set>
#include "direction.h"
#include "map_visitor.h"
#include "visitable_map.h"

class GameObject;
class AvancezLib;
class Sprite;

class Component
{
protected:
	AvancezLib * system;
	GameObject * go;	// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(float dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class SpriteRenderComponent : public Component //For sprites that are the same size as a tile
{

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(float dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
	void setSprite(Sprite * sprite) { this->sprite = sprite; }

protected:
	Sprite* sprite;
};

class GhostSpriteRenderComponent : public SpriteRenderComponent //They are larger than a single tile. 
{
	using SpriteRenderComponent::Update;
	virtual void Update(float dt);
};

class DirectionalSpriteRenderComponent : public Component
{
public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Sprite * sprite, Sprite * sprite_right, Sprite * sprite_left, Sprite * sprite_up, Sprite * sprite_down); // for pac man
	virtual void Update(float dt);
	virtual void Destroy();

private:
	Sprite* sprite;
	Sprite* sprite_left, *sprite_right, *sprite_up, *sprite_down;
	float t = 0;
	bool spriteToggle = false;
};

class RoundedWallRenderComponent : public Component, public MapVisitor
{
public:
	using MapVisitor::setVisitableMap;
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, VisitableMap * map);
	virtual void Update(float dt);
	virtual void setVisitableMap(VisitableMap * map);

private:
	inline bool isWall(VisitableTile * tile);
	VisitableMap * map;
};

class DoorRenderComponent : public Component
{
public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects);
	virtual void Update(float dt);
};

class CollideComponent : public Component
{
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects

public:
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(float dt);
};

