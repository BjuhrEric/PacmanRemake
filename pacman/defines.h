#pragma once
#define WINDOW_WIDTH			1280
#define WINDOW_HEIGHT			720
#define MAP_WIDTH				28
#define MAP_HEIGHT				30
#define TILE_SIZE				22
#define MAX_NUM_GAME_OBJECTS	10000
#define NUM_LIVES				2
#define PLAYER_SPEED			5.0f
#define ENEMY_SPEED				5.0f
#define PLAYER_START_X			6
#define PLAYER_START_Y			13

const int xOffs = (WINDOW_WIDTH - (MAP_WIDTH + 1) * TILE_SIZE) / 2;
const int yOffs = (WINDOW_HEIGHT - (MAP_HEIGHT + 1) * TILE_SIZE) / 2;
const int enemyStartX[4] = { 13, 11, 13, 15 };
const int enemyStartY[4] = { 10, 13, 13, 13 };