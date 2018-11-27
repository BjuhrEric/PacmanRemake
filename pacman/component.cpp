#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include "defines.h"
#include "tile_type.h"

using namespace dir;

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void SpriteRenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(system, go, game_objects);

	sprite = system->createSprite(sprite_name);
}

void SpriteRenderComponent::Update(float dt)
{
	float renderX = go->x * TILE_SIZE + xOffs, renderY = go->y * TILE_SIZE + yOffs;
	Sprite * sprite2 = sprite;
	if (go->enabled && sprite)
		sprite->draw(renderX, renderY);
}

void SpriteRenderComponent::Destroy()
{
	if (sprite != nullptr)
		sprite->destroy();
	sprite = nullptr;
}

void GhostSpriteRenderComponent::Update(float dt)
{
	float renderX = go->x * TILE_SIZE + xOffs + 3, renderY = go->y * TILE_SIZE + yOffs - 10; //Compensating for the larger size.
	Sprite * sprite2 = sprite;
	const Translation * dir_t;
	if (go->enabled && sprite)
	{
		dir_t = getTranslation(go->dir);
		renderX += dir_t->x * go->pMovement * TILE_SIZE;
		renderY += dir_t->y * go->pMovement * TILE_SIZE;
		sprite->draw(renderX, renderY);
	}
}

void DirectionalSpriteRenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, Sprite * sprite, Sprite * sprite_right, Sprite * sprite_left, Sprite * sprite_up, Sprite * sprite_down)
{
	Component::Create(system, go, game_objects);

	this->sprite = sprite;
	this->sprite_right = sprite_right;
	this->sprite_left = sprite_left;
	this->sprite_up = sprite_up;
	this->sprite_down = sprite_down;
}

void DirectionalSpriteRenderComponent::Update(float dt)
{
	float renderX = go->x * TILE_SIZE + xOffs, renderY = go->y * TILE_SIZE + yOffs;
	Sprite * sprite2 = sprite;
	const Translation * dir_t;
	if (!go->enabled)
		return;

	if (go->dir == none && sprite) {
		sprite->draw(renderX, renderY);
	}
	else if (sprite)
	{
		dir_t = getTranslation(go->dir);
		renderX += dir_t->x * go->pMovement * TILE_SIZE;
		renderY += dir_t->y * go->pMovement * TILE_SIZE;
		switch (go->dir) { //Set the other sprite
		case left: sprite2 = sprite_left; break;
		case right: sprite2 = sprite_right; break;
		case up: sprite2 = sprite_up; break;
		case down: sprite2 = sprite_down; break;
		}
		if (sprite && sprite2)
		{
			if (spriteToggle)
				sprite->draw(renderX, renderY);
			if (!spriteToggle)
				sprite2->draw(renderX, renderY);
			t += dt;
			if (t >= 0.25) {
				spriteToggle = !spriteToggle;
				t = 0;
			}
		}
	}

	

	
}

void DirectionalSpriteRenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	if (sprite_left != NULL)
		sprite_left->destroy();
	if (sprite_right != NULL)
		sprite_right->destroy();
	if (sprite_up != NULL)
		sprite_up->destroy();
	if (sprite_down != NULL)
		sprite_down->destroy();
	sprite = NULL;
	sprite_left = NULL;
	sprite_right = NULL;
	sprite_up = NULL;
	sprite_down = NULL;
}

void RoundedWallRenderComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, VisitableMap * map) {
	Component::Create(system, go, game_objects);
	this->map = map;
}

void RoundedWallRenderComponent::Update(float dt) {
	int renderX = go->x * TILE_SIZE + xOffs;
	int renderY = go->y * TILE_SIZE + yOffs;

	VisitableTile *tRight, *tLeft, *tUp, *tDown, *tUpLeft, *tUpRight, *tDownLeft, *tDownRight;
	tRight = map->getTile(go->x + 1, go->y);
	tLeft = map->getTile(go->x - 1, go->y);
	tUp = map->getTile(go->x, go->y - 1);
	tDown = map->getTile(go->x, go->y + 1);
	tUpLeft = map->getTile(go->x - 1, go->y - 1);
	tUpRight = map->getTile(go->x + 1, go->y - 1);
	tDownLeft = map->getTile(go->x - 1, go->y + 1);
	tDownRight = map->getTile(go->x + 1, go->y + 1);


	//Check adjacent types:
	if (tUp)
	{	switch (tUp->getType()) {
		case TILE_TYPE_COIN:
		case TILE_TYPE_EMPTY:
			if (isWall(tLeft))
				system->drawLine(renderX, renderY + TILE_SIZE/2, renderX + TILE_SIZE/2, renderY + TILE_SIZE/2, 0, 0, 255);
			if (isWall(tRight))
				system->drawLine(renderX + TILE_SIZE/2, renderY + TILE_SIZE/2, renderX + TILE_SIZE, renderY + TILE_SIZE/2, 0, 0, 255);
			break;
		case TILE_TYPE_OUTSIDE:
			system->drawLine(renderX, renderY, renderX + TILE_SIZE, renderY, 0, 0, 255);
			break;
		default:
			break;
		}
	}
	else
	{
		system->drawLine(renderX, renderY, renderX + TILE_SIZE, renderY, 0, 0, 255);
	}
	if (tDown)
	{
		switch (tDown->getType()) {
		case TILE_TYPE_COIN:
		case TILE_TYPE_EMPTY:
			if (isWall(tLeft))
				system->drawLine(renderX, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255);
			if (isWall(tRight))
				system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE, renderY + TILE_SIZE / 2, 0, 0, 255);
			break;
		case TILE_TYPE_OUTSIDE:
			system->drawLine(renderX, renderY + TILE_SIZE, renderX + TILE_SIZE, renderY + TILE_SIZE, 0, 0, 255);
			break;
		default:
			break;
		}
	}
	else
	{
		system->drawLine(renderX, renderY + TILE_SIZE, renderX + TILE_SIZE, renderY + TILE_SIZE, 0, 0, 255);
	}
	if (tRight)
	{
		switch (tRight->getType()) {
		case TILE_TYPE_COIN:
		case TILE_TYPE_EMPTY:
			if (isWall(tUp))
				system->drawLine(renderX + TILE_SIZE / 2, renderY, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255);
			if (isWall(tDown))
				system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE, 0, 0, 255);
			break;
		case TILE_TYPE_OUTSIDE:
			system->drawLine(renderX + TILE_SIZE, renderY, renderX + TILE_SIZE, renderY + TILE_SIZE, 0, 0, 255);
			break;
		default:
			break;
		}
	}
	else
	{
		if (isWall(tUp) || isWall(tDown)) //Special case for right corridor
		system->drawLine(renderX + TILE_SIZE, renderY, renderX + TILE_SIZE, renderY + TILE_SIZE, 0, 0, 255);
	}
	if (tLeft)
	{
		switch (tLeft->getType()) {
		case TILE_TYPE_COIN:
		case TILE_TYPE_EMPTY:
			if (isWall(tUp))
				system->drawLine(renderX + TILE_SIZE / 2, renderY, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255);
			if (isWall(tDown))
				system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE, 0, 0, 255);
			break;
		case TILE_TYPE_OUTSIDE:
			system->drawLine(renderX, renderY, renderX, renderY + TILE_SIZE, 0, 0, 255);
			break;
		default:
			break;
		}
	}
	else 
	{
		if (isWall(tUp) || isWall(tDown)) //Special case for left corridor
			system->drawLine(renderX, renderY, renderX, renderY + TILE_SIZE, 0, 0, 255);
	}

	/*
	  Special cases for corners
	*/

	//Down-Right
	if (isWall(tUp) && isWall(tLeft) && (tUpLeft->getType() == TILE_TYPE_COIN || tUpLeft->getType() == TILE_TYPE_EMPTY))
	{
		system->drawLine(renderX, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255); //Left -> Middle: Connect with wall to the left
		system->drawLine(renderX + TILE_SIZE / 2, renderY, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255); //Up -> Middle: Connect with wall above
	}
	//Down-Left
	if (isWall(tUp) && isWall(tRight) && (tUpRight->getType() == TILE_TYPE_COIN || tUpRight->getType() == TILE_TYPE_EMPTY))
	{
		system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE, renderY + TILE_SIZE / 2, 0, 0, 255); //Middle -> Right: Connect with wall to the right
		system->drawLine(renderX + TILE_SIZE / 2, renderY, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255); //Up -> Middle: Connect with wall above
	}
	//Up-Right
	if (isWall(tDown) && isWall(tLeft) && (tDownLeft->getType() == TILE_TYPE_COIN || tDownLeft->getType() == TILE_TYPE_EMPTY))
	{
		system->drawLine(renderX, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255); //Left -> Middle: Connect with wall to the left
		system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE, 0, 0, 255); //Middle -> Down: Connect with wall below
	}
	//Up-Left
	if (isWall(tDown) && isWall(tRight) && (tDownRight->getType() == TILE_TYPE_COIN || tDownRight->getType() == TILE_TYPE_EMPTY))
	{
		system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE, renderY + TILE_SIZE / 2, 0, 0, 255); //Middle -> Right: Connect with wall to the right
		system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE, 0, 0, 255); //Middle -> Down: Connect with wall below
	}

	//Special case for left corridor
	if (!tLeft && isWall(tRight) && !isWall(tUp) && !isWall(tDown))
	{
		system->drawLine(renderX, renderY + TILE_SIZE / 2, renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, 0, 0, 255);
		if (tUp && tUp->getType() == TILE_TYPE_EMPTY)
			system->drawLine(renderX, renderY + TILE_SIZE / 2, renderX, renderY + TILE_SIZE, 0, 0, 255);
		if (tDown && tDown->getType() == TILE_TYPE_EMPTY)
			system->drawLine(renderX, renderY, renderX, renderY + TILE_SIZE / 2, 0, 0, 255);
	}

	//Special case for right corridor
	if (!tRight && isWall(tLeft) && !isWall(tUp) && !isWall(tDown))
	{
		system->drawLine(renderX + TILE_SIZE / 2, renderY + TILE_SIZE / 2, renderX + TILE_SIZE, renderY + TILE_SIZE / 2, 0, 0, 255);
		if (tUp && tUp->getType() == TILE_TYPE_EMPTY)
			system->drawLine(renderX + TILE_SIZE, renderY + TILE_SIZE / 2, renderX + TILE_SIZE, renderY + TILE_SIZE, 0, 0, 255);
		if (tDown && tDown->getType() == TILE_TYPE_EMPTY)
			system->drawLine(renderX + TILE_SIZE, renderY, renderX + TILE_SIZE, renderY + TILE_SIZE / 2, 0, 0, 255);
	}
}

inline bool RoundedWallRenderComponent::isWall(VisitableTile * tile) {
	if (!tile)
		return false;
	const Type type = tile->getType();
	return type == TILE_TYPE_ROUNDED_WALL || type == TILE_TYPE_STRAIGHT_WALL || type == TILE_TYPE_DOOR;
}

void RoundedWallRenderComponent::setVisitableMap(VisitableMap * map) {
	this->map = map;
}

void DoorRenderComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects) 
{
	Component::Create(system, go, game_objects);
}

void DoorRenderComponent::Update(float dt) 
{
	int renderX = go->x * TILE_SIZE + xOffs;
	int renderY = go->y * TILE_SIZE + yOffs;
	system->drawLine(renderX, renderY + TILE_SIZE/2, renderX + TILE_SIZE, renderY +  TILE_SIZE/2, 255, 0, 0);
}

void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}

void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go = coll_objects->pool[i];
		if (go->enabled)
		{
			if (go->x == this->go->x && go->y == this->go->y)
			{
				this->go->Receive(HIT);
				go->Receive(HIT);
			}
		}
	}
}
