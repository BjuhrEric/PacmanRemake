#pragma once
#include "direction.h"
#include "coordinates.h"

class MovablePosition : public Coordinates
{
public:

	virtual Direction getDirection() {
		return dir;
	}

	Direction dir = none;
};