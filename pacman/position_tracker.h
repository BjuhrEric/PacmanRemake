#pragma once
#include "movable_position.h"

class PositionTracker 
{
public:
	virtual void trackPosition(MovablePosition * track) = 0;
};