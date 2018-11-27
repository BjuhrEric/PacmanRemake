// GameObject represents objects which moves are drawn
#include <vector>
#include "direction.h"
#include "movable_position.h"
#include "coordinates.h"

enum Message { HIT, GAME_OVER };

class Component;

class GameObject : public MovablePosition
{
protected:
	std::vector<GameObject*> receivers;
	std::vector<Component*> components;

public:
	float pMovement = 0;
	bool enabled;
	bool active = false;

	virtual ~GameObject();

	virtual void Create();
	virtual void AddComponent(Component * component);

	using MovablePosition::getDirection;
	virtual Direction getDirection();

	virtual void Init();
	virtual void Update(float dt);
	virtual void Destroy();
	virtual void AddReceiver(GameObject *go);
	virtual void Receive(Message m) {}
	void Send(Message m);
};