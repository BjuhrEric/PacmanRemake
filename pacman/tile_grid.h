#pragma once
#include "visitable_map.h"
#include "visitable_scorekeeper.h"
#include "scorekeeper_visitor.h"
#include "tile_type.h"
#include "direction.h"
#include "coordinates.h"
#include "path_comparator.h"
#include <queue>

class TileGridBehaviourComponent : public Component
{
public:
	virtual ~TileGridBehaviourComponent() {
	
	}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects) {
		Component::Create(system, go, game_objects);
	}

	virtual void Update(float dt)
	{
		//Power ups
	}
};

class TileGrid : public GameObject, public VisitableMap
{
public:

	virtual ~TileGrid() { }

	using VisitableMap::isIntersection;
	bool isIntersection(int x, int y) {
		int accessibleDirections = 0;
		if (isAccessible(x + 1, y)) accessibleDirections += 1;
		if (isAccessible(x - 1, y)) accessibleDirections += 1;
		if (isAccessible(x, y + 1)) accessibleDirections += 1;
		if (isAccessible(x, y - 1)) accessibleDirections += 1;
		return accessibleDirections > 2;
	}

	using VisitableMap::getNextIntersection;
	VisitableTile * getNextIntersection(Coordinates & from, const Direction dir) {
		Direction curDir = dir;
		int x = from.getX() + getTranslation(dir)->x,  y = from.getY() + getTranslation(dir)->y;
		while (!isIntersection(x, y)) {
			nextCorridorPosition(x, y, curDir);
		}
		return getTile(x, y);
	}

	void nextCorridorPosition(int & x, int & y, Direction & dir) {
		const Translation * const t = getTranslation(dir);
		if (!isAccessible(x + t->x, y + t->y) || dir == none) {
			if (t->x != 0 || dir == none) 
			{
				//We were moving horizontally. Now move vertically
				if (isAccessible(x, y + 1))
				{
					y += 1;
					dir = down;
					return;
				}
				if (isAccessible(x, y - 1))
				{
					y -= 1;
					dir = up;
					return;
				}
			}
			
			if (t->y != 0 || dir == none) {
				//Vice versa
				if (isAccessible(x + 1, y))
				{
					x += 1;
					dir = right;
					return;
				}
				if (isAccessible(x - 1, y))
				{
					x -= 1;
					dir = left;
					return;
				}
			}
		}
		else 
		{
			//Just move forward
			movePositionWrapAround(x, y, t);
		}
	}
	
	inline void movePositionWrapAround(int &x, int &y, const Translation * const t) {
		x = (x + t->x + MAP_WIDTH) % MAP_WIDTH;
		y += t->y;
	}

	using VisitableMap::shortestPath;
	Path shortestPath(Coordinates& from, Coordinates& to, list<Coordinates>& ignore)
	{
		//Dijkstra, modified to ignore specified tiles even if there is an edge.
		//The ignore will be used to enforce pathing from a specific direction.
		bool visited[MAP_WIDTH][MAP_HEIGHT];
		Path noPath;
		priority_queue<Path> pq;

		Tile * s = getTile(from.x, from.y);
		Tile * e = getTile(to.x, to.y);

		if (s == nullptr)
			return noPath;

		if (e == nullptr)
			e = getClosestAccessibleTile(to);

		if (e == nullptr)
			return noPath; //Something went wrong! Breakpoint here.

		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
				visited[x][y] = false;
		
		
		// Setting the ignored coordinates to visited means they wont be visited by Dijkstra's algorithm.
		if (!ignore.empty())
			for (auto coord = ignore.begin(); coord != ignore.end(); coord++)
			{
				Tile * tile = getTile(coord->x, coord->y);
				if (tile)
					visited[tile->x][tile->y] = true;
			}

		//Set up initial paths.
		const Edge * edge;
		for (int i = 0; i < 4; i++)
		{
			edge = s->getEdge((Direction) i);
			if (edge)
			{
				if (!visited[edge->to->getX()][edge->to->getY()]) {
					Path p;
					p.path.push_back(*edge);
					p.length += 1;
					pq.push(p);
					visited[edge->to->getX()][edge->to->getY()] = true;
				}
			}
		}

		while (pq.size() > 0)
		{
			Path shortPath = pq.top();
			pq.pop();
			
			const Edge last = shortPath.path.back();
			Tile * const to = getTile(last.to->getX(), last.to->getY());
			if (to == e)
				return shortPath;

			const Edge * edge;
			for (int i = 0; i < 4; i++)
			{
				edge = to->getEdge((Direction)i);
				if (edge)
				{
					//If the tile has already been visited, this is a suboptimal path to the tile
					if (!visited[edge->to->getX()][edge->to->getY()]) 
					{
						Path p(shortPath);
						p.path.push_back(*edge);
						p.length += 1;
						pq.push(p);
						visited[edge->to->getX()][edge->to->getY()] = true;
					}
				}
			}
		}
		return noPath;
	}

	Tile * getClosestAccessibleTile(Coordinates& to) {
		//Find closest accessible tile
		bool v[MAP_WIDTH][MAP_HEIGHT];
		for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
				v[x][y] = false;

		queue<Coordinates> q;
		Coordinates start(to);
		start.x = min(MAP_WIDTH, start.x);
		start.x = max(0, start.x);
		start.y = min(MAP_HEIGHT, start.y);
		start.y = max(0, start.y);

		q.push(start);
		while (!q.empty())
		{
			Coordinates curr = q.front();
			Coordinates left(curr); left.x -= 1;
			Coordinates right(curr); right.x += 1;
			Coordinates up(curr); up.y -= 1;
			Coordinates down(curr); down.y += 1;
			Tile * t;

			q.pop();
			
			if (t = checkAdjacent(left, v, &q))
				return t;
			if (t = checkAdjacent(right, v, &q))
				return t;
			if (t = checkAdjacent(up, v, &q))
				return t;
			if (t = checkAdjacent(down, v, &q))
				return t;
		}
		return nullptr;
	}

	

	using VisitableMap::isAccessible;
	bool isAccessible(int x, int y) {
		if (y != 13 && (x < 0 || x >= MAP_WIDTH))
			return false;
		if (y == 13 && (x < 0 || x >= MAP_WIDTH))
			return true;
		if (y < 0 || y >= MAP_HEIGHT)
			return false;
		Type type = tiles[y][x]->getType();
		if (type == TILE_TYPE_ROUNDED_WALL || type == TILE_TYPE_STRAIGHT_WALL) {
			return false;
		}
		return true;
	}

	using VisitableMap::getTile;
	Tile * getTile(int x, int y)
	{
		if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
			return tiles[y][x];
		if (x < 0 && y == 13)
			return tiles[13][MAP_WIDTH - 1];
		if (x >= MAP_WIDTH && y == 13)
			return tiles[13][0];
		return nullptr;
	}

	inline bool isWall(Tile * tile) {
		if (!tile)
			return false;
		const Type type = tile->getType();
		return type == TILE_TYPE_ROUNDED_WALL || type == TILE_TYPE_STRAIGHT_WALL;
	}

	virtual void Create(AvancezLib* system, std::set<GameObject*> * game_objects, ObjectPool<Tile> * tile_pool)
	{
		GameObject::Create();
		this->system = system;
		this->game_objects = game_objects;
		this->tile_pool = tile_pool;
	}

	virtual void Init(VisitableScoreKeeper * scoreKeeper)
	{
		int i = 0;
		for (char y = 0; y < 30; y++)
			for (char x = 0; x < 28; x++)
			{
				char tiletype = map[y][x];
				Tile * tile = tile_pool->FirstAvailable();
				tile->setType(tiletype);
				tile->setID(i);
				tile->enable();
				tiles[y][x] = tile;
				i++;
				switch (tile->getType()) {

				case TILE_TYPE_COIN:
				{
					SpriteRenderComponent * tile_render = new SpriteRenderComponent();
					tile_render->Create(system, tile, game_objects, "data/testcoin.bmp");
					tile->AddComponent(tile_render);
					tile->setVisitableScoreKeeper(scoreKeeper);
					scoreKeeper->addMaxScore(100);
					break;
				}
				case TILE_TYPE_ROUNDED_WALL: //For now, both of them are straight walls. One will have rounded edges later on.
				case TILE_TYPE_STRAIGHT_WALL:
				{
					RoundedWallRenderComponent * tile_render = new RoundedWallRenderComponent();
					tile_render->Create(system, tile, game_objects, this);
					tile->AddComponent(tile_render);
					break;
				}
				case TILE_TYPE_DOOR:
				{
					DoorRenderComponent * tile_render = new DoorRenderComponent();
					tile_render->Create(system, tile, game_objects);
					tile->AddComponent(tile_render);
					break;
				}
				case TILE_TYPE_EMPTY: //After a cell has been eaten
				case TILE_TYPE_OUTSIDE: //To be able to define outer walls
				default:
					break;
				}

				tile->Init(x, y);
				game_objects->insert(tile);
			}
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				findEdges(x, y);
			}
		}
	}

	void findEdges(int x, int y)
	{
		Tile * to;
		Tile * from = tiles[y][x];
		if (isAccessible(x - 1, y)) //left
		{
			to = getTile(x - 1,y);
			const Edge * e = new Edge(from, to, left);
			from->setEdge(left, e);
		}

		if (isAccessible(x + 1, y)) //right
		{
			to = getTile(x + 1, y);
			const Edge * e = new Edge(from, to, right);
			from->setEdge(right, e);
		}

		if (isAccessible(x, y - 1)) //up
		{
			to = getTile(x, y - 1);
			const Edge * e = new Edge(from, to, up);
			from->setEdge(up, e);
		}

		if (isAccessible(x, y + 1)) //down
		{
			to = getTile(x, y + 1);
			const Edge * e = new Edge(from, to, down);
			from->setEdge(down, e);
		}
	}

private:
	bool change_direction;

	ObjectPool<Tile> * tile_pool;
	const char map[30][28] =
	{
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 3, 4, 4, 4, 5, 5, 4, 4, 4, 3, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 4, 2, 2, 2, 2, 2, 2, 4, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1 },
		{ 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 4, 2, 2, 2, 2, 2, 2, 4, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3 },
		{ 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 4, 2, 2, 2, 2, 2, 2, 4, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 2, 2, 2, 2, 2, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 2, 2, 2, 2, 2 },
		{ 1, 1, 1, 1, 1, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1 },
		{ 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1 },
		{ 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
	Tile* tiles[30][28];
	AvancezLib * system;
	std::set<GameObject*> * game_objects;

	inline Tile * checkAdjacent(Coordinates check, bool v[MAP_WIDTH][MAP_HEIGHT], queue<Coordinates> * q)
	{
		// used only for getClosestAccessibleTile
		Tile * t = getTile(check.x, check.y);
		if (t && !v[check.x % MAP_WIDTH][check.y]) // Taking the corridor wrap around into account
		{
			if (isAccessible(check.x, check.y))
				return t;

			v[check.x][check.y] = true;
			q->push(check);
		}

		return nullptr;
	}

};