#include "visitable_map.h"
#include "map_visitor.h"
#include "visitable_scorekeeper.h"

class Game : public GameObject, public VisitableScoreKeeper
{
	std::set<GameObject*> game_objects;
	
	AvancezLib* system;
	bool openedDoor = false;
	bool restart = false;
	ObjectPool<Enemy> enemy_pool;
	ObjectPool<Tile> tile_pool;
	Enemy *blinky, *pinky, *inky, *clyde;

	Player * player;
	TileGrid * tile_grid;

	Sprite * sprite_right;
	Sprite * sprite_left;
	Sprite * sprite_up;
	Sprite * sprite_down;
	Sprite * sprite_closed;
	bool game_over = false;

	unsigned int score = 0;
	unsigned int lastScore = -1;
	unsigned int maxScore = 0;
	SDL_Texture * scoreTexture = nullptr;

public:
	using VisitableScoreKeeper::addScore;
	void addScore(int amount) 
	{
		score += amount;
	}

	using VisitableScoreKeeper::addMaxScore;
	void addMaxScore(int amount)
	{
		maxScore += amount;
	}

	virtual void Create(AvancezLib* system)
	{

		this->system = system;

		sprite_right = system->createSprite("data/pacman/pacmanopen_right.bmp");
		sprite_left = system->createSprite("data/pacman/pacmanopen_left.bmp");
		sprite_up = system->createSprite("data/pacman/pacmanopen_up.bmp");
		sprite_down = system->createSprite("data/pacman/pacmanopen_down.bmp");
		sprite_closed = system->createSprite("data/pacman/pacmanclosed.bmp");
		
		CreateTiles();
		CreateMap();
		CreatePlayer();
		CreateEnemies();

		score = 0;
	}

	inline virtual void CreateTiles() {
		tile_pool.Create(MAP_WIDTH*MAP_HEIGHT);
		for (auto tile = tile_pool.pool.begin(); tile != tile_pool.pool.end(); tile++)
		{
			(*tile)->Create();
			(*tile)->AddReceiver(this);
		}
	}

	inline virtual void CreateMap() {
		tile_grid = new TileGrid();
		TileGridBehaviourComponent  * tile_grid_behaviour = new TileGridBehaviourComponent();
		tile_grid_behaviour->Create(system, tile_grid, &game_objects);
		tile_grid->Create(system, &game_objects, &tile_pool);
		tile_grid->AddComponent(tile_grid_behaviour);
		game_objects.insert(tile_grid);
	}

	inline virtual void CreatePlayer() {
		player = new Player();
		PlayerBehaviourComponent * player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(system, player, &game_objects);
		DirectionalSpriteRenderComponent * player_render = new DirectionalSpriteRenderComponent();
		player_render->Create(system, player, &game_objects, sprite_closed, sprite_right, sprite_left, sprite_up, sprite_down);
		CollideComponent * player_enemy_collision = new CollideComponent();
		player_enemy_collision->Create(system, player, &game_objects, (ObjectPool<GameObject>*) &enemy_pool);

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(player_render);
		player->AddComponent(player_enemy_collision);
		player->AddReceiver(this);

		player_behaviour->setVisitableMap(tile_grid);
	}

	inline virtual void CreateEnemies() {
		enemy_pool = *new ObjectPool<Enemy>();
		enemy_pool.Create(4);
		char i = 0;
		for (auto enemy = enemy_pool.pool.begin(); enemy != enemy_pool.pool.end(); enemy++)
		{
			GhostSpriteRenderComponent * enemy_render = new GhostSpriteRenderComponent();
			enemy_render->Create(system, *enemy, &game_objects, *(i + enemyTexture));

			EnemyBehaviourComponent * enemy_behavior = nullptr;
			switch (i)
			{
			case 0: enemy_behavior = new BlinkyBehaviourComponent(); blinky = *enemy; break;
			case 1: enemy_behavior = new InkyBehaviourComponent(); inky = *enemy; break;
			case 2: enemy_behavior = new PinkyBehaviourComponent(); pinky = *enemy; break;
			case 3: enemy_behavior = new ClydeBehaviourComponent(); clyde = *enemy; break;
			default: enemy_behavior = new BlinkyBehaviourComponent(); break;
			}

			enemy_behavior->Create(system, *enemy, &game_objects);
			enemy_behavior->setVisitableMap(tile_grid);
			enemy_behavior->trackPosition(player);
			if (i == 1)
			{
				enemy_behavior->trackPosition(blinky); //Inky is tracking Blinky's position.
			}
			(*enemy)->Create();
			(*enemy)->AddComponent(enemy_render);
			(*enemy)->AddComponent(enemy_behavior);
			(*enemy)->AddReceiver(this);
			i++;
		}
	}

	virtual void Init()
	{
		player->Init();
		int i = 0;
		tile_grid->Init(this);
		for (auto enemy = enemy_pool.pool.begin(); enemy != enemy_pool.pool.end(); enemy++)
		{
			(*enemy)->Init(enemyStartX[i], enemyStartY[i]);
			i++;
		}
		enabled = true;


		blinky->active = true;
		pinky->active = true;
	}

	virtual void reset()
	{
		score = 0;
		maxScore = 0;
		tile_pool.DisableAll();
		system->reset();
		Init();
	}

	virtual void Update(float dt)
	{
		if (restart)
		{
			reset();
			openedDoor = false;
			system->setStarted(false);
			player->RemoveLife();
			restart = false;
		}

		if (score >= 3000)
			inky->active = true;

		if ((double) score / (double) maxScore > 0.33)
			clyde->active = true;

		if (!system->hasStarted() || system->isPaused() || IsGameOver())
			dt = 0.f;
		if (!openedDoor && system->hasStarted())
		{
			openedDoor = true;
			tile_grid->getTile(13, 11)->clearComponents();
			tile_grid->getTile(14, 11)->clearComponents();
		}

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		for (auto enemy = enemy_pool.pool.begin(); enemy != enemy_pool.pool.end(); enemy++)
			(*enemy)->Update(dt);

		if (!IsGameOver())
			player->Update(dt);
	}

	virtual void Draw()
	{
		unsigned int score = Score();
		if (score != lastScore) {
			char msg[1024];
			sprintf(msg, "%07d", score);
			if (scoreTexture != nullptr) {
				system->destroyTexture(scoreTexture);
			}
			scoreTexture = system->createTextTexture(msg);
		}

		lastScore = score;
		system->drawTexture(20, 50, scoreTexture);

		if (sprite_right)
			for (int i = 0; i < player->lives; i++)
				sprite_right->draw(i*36+20, 16);

		if (IsGameOver())
		{
			system->drawTexture(WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT - 20, system->gameOverTexture);
		}
	}

	virtual void Receive(Message m)
	{
		if (m == GAME_OVER)
			game_over = true;
		if (m == HIT)
		{
			restart = true;
		}
	}


	bool IsGameOver()
	{
		return game_over;
	}

	unsigned int Score()
	{
		return score;
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		enemy_pool.Destroy();
		tile_pool.Destroy();
		delete tile_grid;
		delete player;
	}
};