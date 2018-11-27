#define _CRT_SECURE_NO_WARNINGS

#include "defines.h"
#include "texturepaths.h"
#include "stdio.h"

#include <set>

#include "avancezlib.h"
#include "object_pool.h"
#include "component.h"
#include "game_object.h"
#include "player.h"
#include "enemy.h"
#include "tile.h"
#include "tile_grid.h"
#include "game.h"
#include "direction.h"
#include "state.h"
#include "visitable_map.h"
#include "map_visitor.h"
#include "visitable_scorekeeper.h"
#include "scorekeeper_visitor.h"
#include "movable_position.h"
#include "position_tracker.h"
#include "movement_component.h"
#include "edge.h"
#include "path.h"
#include "coordinates.h"

int main(int argc, char** argv)
{

	AvancezLib system;

	system.init(WINDOW_WIDTH, WINDOW_HEIGHT);

	Game game;
	game.Create(&system);
	game.Init();

	float lastTime = system.getElapsedTime();
	while (system.update())
	{
		float newTime = system.getElapsedTime();
		float dt = newTime - lastTime;
		lastTime = newTime;

		game.Update(dt);

		game.Draw();
	}

	// clean up
	game.Destroy();
	system.destroy();

	return 0;
}



