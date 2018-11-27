#include "avancezlib.h"


// Creates the main window. Returns true on success.
bool AvancezLib::init(int width, int height)
{
	this->width = width;
	this->height = height;

	SDL_Log("Initializing the system...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("aVANCEZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("data/space_invaders.ttf", 12); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// initialize the keys
	dir = none;

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("System up and running...\n");
	SDL_Log("Creating common textures...\n");
	pauseTexture1 = createTextTexture("Paused! Press space to continue.");
	pauseTexture2 = createTextTexture("Press any key to start the game! GLHF!");
	gameOverTexture = createTextTexture("*** G A M E  O V E R ***");
	return true;
}


// Destroys the avancez library instance
void AvancezLib::destroy()
{
	SDL_Log("Shutting down the system\n");
	destroyTexture(pauseTexture1);
	destroyTexture(pauseTexture2);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}


bool AvancezLib::update()
{
	bool quit = false;
	SDL_Event event;


	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			quit = true;

		if (event.type == SDL_KEYDOWN)
		{
			if (!hasStarted())
				start = true;
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				quit = true;
				break;
			case SDLK_LEFT:
				dir = Direction::left;
				break;
			case SDLK_RIGHT:
				dir = Direction::right;
				break;
			case SDLK_UP:
				dir = Direction::up;
				break;
			case SDLK_DOWN:
				dir = Direction::down;
				break;
			case SDLK_PAUSE:
			case SDLK_SPACE:
				pause = !pause;
				break;
			}
		}

	}

	if (pause)
		drawTexture(width / 2 - 100, height - 20, pauseTexture1);
	if (!hasStarted())
		drawTexture(width / 2 - 100, height - 20, pauseTexture2);

	//Update screen
	SDL_RenderPresent(renderer);

	//Clear screen
	SDL_RenderClear(renderer);

	return !quit;
}


Sprite * AvancezLib::createSprite(const char * path)
{
	SDL_Surface* surf = SDL_LoadBMP(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite * sprite = new Sprite(renderer, texture);

	return sprite;
}

SDL_Texture * AvancezLib::createTextTexture(const char * const msg) {
	SDL_Color white = { 255, 255, 255 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Surface * textSurface = TTF_RenderText_Solid(font, msg, white); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, textSurface); //now you can convert it into a texture
	SDL_FreeSurface(textSurface); /* we got the texture now -> free surface */
	return texture;
}

inline void AvancezLib::destroyTexture(SDL_Texture * texture) {
	SDL_DestroyTexture(texture);
}


void AvancezLib::drawTexture(int x, int y, SDL_Texture * const texture)
{
	int w = 0;
	int h = 0;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	if (x < 0-abs(w) || y < 0-abs(h) || x > width || y > width)
		return;
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, texture, NULL, &dst_rect);

}

void AvancezLib::drawLine(int x1, int y1, int x2, int y2, int r, int g, int b) 
{
	Uint8 tmpR, tmpG, tmpB, tmpA;
	tmpR = tmpG = tmpB = tmpA = 0;
	SDL_GetRenderDrawColor(renderer, &tmpR, &tmpG, &tmpB, &tmpA);
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	SDL_SetRenderDrawColor(renderer, tmpR, tmpG, tmpB, tmpA);
}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

Direction AvancezLib::getDirection()
{
	return dir;
}

State AvancezLib::getPlayerState() {
	return playerState;
}

State AvancezLib::getEnemyState() {
	return enemyState;
}

int AvancezLib::getWidth() {
	return width;
}

int AvancezLib::getHeight() {
	return height;
}

void AvancezLib::reset() {
	dir = none;
}


Sprite::Sprite(SDL_Renderer * renderer, SDL_Texture * texture)
{
	this->renderer = renderer;
	this->texture = texture;
}


void Sprite::draw(int x, int y)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	//Render texture to screen
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}


void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}