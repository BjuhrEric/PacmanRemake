#pragma once
#include "tile_type.h"

class VisitableTile {
public:
	virtual void visit() = 0;
	virtual Type getType() = 0;
	virtual int getX() = 0;
	virtual int getY() = 0;
};