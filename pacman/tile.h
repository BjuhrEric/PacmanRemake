#pragma once
#include "visitable_tile.h"
#include "visitable_scorekeeper.h"
#include "scorekeeper_visitor.h"
#include "tile_type.h"
#include "edge.h"


class Tile : public GameObject, public VisitableTile, public ScoreKeeperVisitor {
public:

	Tile()
	{
		edges[0] = nullptr;
		edges[1] = nullptr;
		edges[2] = nullptr;
		edges[3] = nullptr;
	}

	virtual void Init(double X, double Y)
	{
		x = X;
		y = Y;
	}

	using ScoreKeeperVisitor::setVisitableScoreKeeper;
	void setVisitableScoreKeeper(VisitableScoreKeeper * scoreKeeper) {
		this->scoreKeeper = scoreKeeper;
	}

	void enable() {
		enabled = true;
	}

	const Edge * getEdge(Direction dir)
	{
		if (dir == none) return nullptr;
		return edges[dir];
	}

	int getID() {
		return id;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	void clearComponents()
	{
		components.clear();
	}

	using VisitableTile::getType;
	Type getType() {
		return type;
	}

	void setEdge(const Direction dir, const Edge * e)
	{
		if (dir != none)
			edges[dir] = e;
	}

	void setID(int id) {
		this->id = id;
	}
	void setType(Type type) {
		this->type = type;
	}

	void setType(char type) {
		if (type == 0) setType(TILE_TYPE_COIN);
		if (type == 1) setType(TILE_TYPE_ROUNDED_WALL);
		if (type == 2) setType(TILE_TYPE_OUTSIDE);
		if (type == 3) setType(TILE_TYPE_EMPTY);
		if (type == 4) setType(TILE_TYPE_STRAIGHT_WALL);
		if (type == 5) setType(TILE_TYPE_DOOR);
	}

	using VisitableTile::visit;
	void visit() {
		if (type == TILE_TYPE_COIN) {
			type = TILE_TYPE_EMPTY;
			if (scoreKeeper)
				scoreKeeper->addScore(100);
			components.clear();
		}
	}

private:
	int id;
	Type type;
	VisitableScoreKeeper * scoreKeeper;
	const Edge * edges[4];
};

