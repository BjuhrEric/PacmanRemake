#pragma once
#include "visitable_tile.h"

struct Edge {
	VisitableTile * const to;
	VisitableTile * const from;
	const Direction dir;

	Edge(VisitableTile * from, VisitableTile * to, Direction dir) : from(from), to(to), dir(dir) {}
};