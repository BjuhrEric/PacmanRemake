#pragma once
#include "visitable_tile.h"
#include "path.h"
#include "direction.h"
#include "coordinates.h"

class VisitableMap {
public:
	virtual bool isAccessible(int x, int y) = 0;
	virtual VisitableTile * getTile(int x, int y) = 0;
	virtual Path shortestPath(Coordinates& from, Coordinates& to, list<Coordinates>& ignore) = 0;
	virtual bool isIntersection(int x, int y) = 0;
	virtual VisitableTile * getNextIntersection(Coordinates& from, Direction dir) = 0;
};