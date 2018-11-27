#pragma once
#include "movement_component.h"
class PlayerBehaviourComponent : public MovementComponent
{

public:
	virtual ~PlayerBehaviourComponent() {}

	void Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects) {
		MovementComponent::Create(system, go, game_objects, PLAYER_SPEED);
	}

	virtual void Init()
	{
		go->x = PLAYER_START_X;
		go->y = PLAYER_START_Y;
		go->dir = none;
	}

	virtual Direction getNextDirection()
	{
		return system->getDirection();
	}
};


// the main player
class Player : public GameObject
{
public:

	int lives = NUM_LIVES;	// it's game over when goes below zero 

	virtual ~Player()	{		SDL_Log("Player::~Player");	}

	virtual void Init()
	{
		GameObject::Init();
		active = true;
	}

	virtual void Receive(Message m) 
	{
		if (m == HIT)
		{ 
			if (lives <= 0)
				Send(GAME_OVER);
			else
				Send(HIT);
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};
