#ifndef __AVANCEZ_LIB__
#define __AVANCEZ_LIB__

#include "SDL.h"
#include "SDL_ttf.h"

#include "direction.h"
#include "state.h"

class Sprite
{
public:
	Sprite(SDL_Renderer * renderer, SDL_Texture * texture);

	// Destroys the sprite instance
	void destroy();

	// Draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
	void draw(int x, int y);

private:
	SDL_Renderer * renderer;
	SDL_Texture * texture;
};


class AvancezLib
{
public:
	SDL_Texture * pauseTexture1, *pauseTexture2, *gameOverTexture;
	// Destroys the avancez library instance
	void destroy();

	// Creates the main window. Returns true on success.
	bool init(int width, int height);

	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.
	bool update();

	void setStarted(bool start) { this->start = start; }

	bool hasStarted() { return start; }

	bool isPaused() { return pause; }

	// Create a sprite given a string.
	// All sprites are 32*32 pixels.
	Sprite* createSprite(const char* name);
	SDL_Texture* createTextTexture(const char * const msg);
	void destroyTexture(SDL_Texture * texture);

	// Draws the given text.
	void drawTexture(int x, int y, SDL_Texture * const texture);

	void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b);

	// Return the total time spent in the game, in seconds.
	float getElapsedTime();

	// Returns the next direction, as defined by last directional key struck.
	Direction getDirection();
	// Returns the current player state. Not sure it should be here.
	State getPlayerState();
	State getEnemyState();

	// Returns the size of the window
	int getWidth();
	int getHeight();

	void reset();
private:
	bool start;
	bool pause;
	int width;
	int height;
	SDL_Window * window;
	SDL_Renderer * renderer;

	TTF_Font* font;

	Direction dir = Direction::none;
	State playerState = State::hunted;
	State enemyState = State::hunter;
};





#endif // __AVANCEZ_LIB__
