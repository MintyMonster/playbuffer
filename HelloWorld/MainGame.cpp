#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <string>

int DISPLAY_WIDTH = 450;
int DISPLAY_HEIGHT = 300;
int DISPLAY_SCALE = 2;
int PLAYER;
int ASTEROID;
int SMALL_ASTEROID;
int SPACESHIP;
int angle = 1; // 1 = up, 2 = down, 3 = left, 4 = right;

// Player shooting timer
float startTime = 0;
float currentTime = 0;
float delay = 0.05f;

// Asteroid spawn timer
float asteroidStart = 0;
float asteroidCurrent = 0;
float asteroidDelay = 1.0f;

using namespace Play;

struct GameState {
	int score = 0;
	float time = 0;
};

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_PLAYER,
	TYPE_ASTEROID,
	TYPE_SPACESHIP,
	TYPE_SMALL_ASTEROID,
	TYPE_BULLET,
	TYPE_DESTROYED,
};

GameState gamestate;

void HandlePlayerControls() {

	GameObject& obj_player = GetGameObjectByType(TYPE_PLAYER);

	if (KeyDown(0x57)) { // up
		obj_player.velocity = { 0,-1 };
		SetSprite(obj_player, "playerChar", 0.2f);
		angle = 1;
	}
	else if (KeyDown(0x53)) { // down
		obj_player.velocity = { 0, 1 };
		SetSprite(obj_player, "playerChar_down", 0.2f);
		angle = 2;
	}
	else if (KeyDown(0x44)) { // right
		obj_player.velocity = { 1, 0 };
		SetSprite(obj_player, "playerChar_right", 0.2f);
		angle = 4;
	}
	else if (KeyDown(0x41)) { // right
		obj_player.velocity = { -1, 0 };
		SetSprite(obj_player, "playerChar_left", 0.2f);
		angle = 3;
	}
	else {
		obj_player.velocity *= 0.5f;
		obj_player.acceleration = { 0, 0 };
	}

	if (KeyDown(VK_SPACE)) {
		/*
		if (currentTime - startTime > delay) {
			startTime = currentTime;
			currentTime = 0;

			
		} */

		Point2D shotAngle;

		if (angle == 1)
			shotAngle = { 0, -3 };
		else if (angle == 2)
			shotAngle = { 0, 3 };
		else if (angle == 3)
			shotAngle = { -3, 0 };
		else if (angle == 4)
			shotAngle = { 3, 0 };

		Vector2D firePos = obj_player.pos;
		int id = CreateGameObject(TYPE_BULLET, firePos, 5, "bullet");
		GetGameObject(id).velocity = shotAngle;
	}
	
	UpdateGameObject(obj_player);
	DrawObjectRotated(obj_player);

}

void UpdateShots() {
	std::vector<int> shots = CollectGameObjectIDsByType(TYPE_BULLET);
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID);
	std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID);

	for (int id_shot : shots) {

		GameObject& obj_shot = GetGameObject(id_shot);
		bool hasCollided = false;

		for (int id_asteroid : asteroids) {
			GameObject& obj_asteroid = GetGameObject(id_asteroid);
			if (IsColliding(obj_shot, obj_asteroid)) {
				hasCollided = true;
				obj_asteroid.type = TYPE_DESTROYED;
				gamestate.score += 100;
			}
		}

		for (int id_smallAsteroid : smallAsteroids) {
			GameObject& obj_ast = GetGameObject(id_smallAsteroid);
			if (IsColliding(obj_shot, obj_ast)) {
				hasCollided = true;
				obj_ast.type = TYPE_DESTROYED;
				gamestate.score += 100;
			}
		}

		UpdateGameObject(obj_shot);
		DrawObject(obj_shot);

		if (!IsVisible(obj_shot) || hasCollided)
			DestroyGameObject(id_shot);
	}
}

void UpdateDestroyed() {
	std::vector<int> dead = CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int id_dead : dead) {
		GameObject& obj_dead = GetGameObject(id_dead);
		obj_dead.animSpeed = 0.2f;
		UpdateGameObject(obj_dead);

		if (obj_dead.frame % 2)
			DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);

		if (!IsVisible(obj_dead) || obj_dead.frame >= 10)
			DestroyGameObject(id_dead);
	}
}

void SpawnAsteroids() {
	if (asteroidCurrent - asteroidStart > asteroidDelay) {
		asteroidStart = asteroidCurrent;
		asteroidCurrent = 0;

		int where = 0;
		where = RandomRollRange(0, 4);
		Vector2D spawnPos;

		if (where == 0) {
			// left
			// {0, 1-300}
			spawnPos = { 0, RandomRollRange(1, 300) };
			int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 20, "asteroid1");
			GetGameObject(id).velocity = { 2, 0 };
		}
		else if (where == 1) {
			// top
			// {1-450, 0}
			spawnPos = { RandomRollRange(1, 450), 0 };
			int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 20, "asteroid1");
			GetGameObject(id).velocity = { 0, 2 };
		}
		else if (where == 2) {
			// right
			// {450, 1-300} 
			spawnPos = { 450, RandomRollRange(1, 300) };
			int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 20, "asteroid1");
			GetGameObject(id).velocity = { -2, 0 };
		}
		else {
			// bottom
			// {1-450, 300}
			spawnPos = { RandomRollRange(1, 450), 300 };
			int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 20, "asteroid1");
			GetGameObject(id).velocity = { 0, -2 };
		}
	}
}

void UpdateAsteroids() {
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID);

	for (int id_asteroid : asteroids) {
		GameObject& obj_asteroid = GetGameObject(id_asteroid);

		UpdateGameObject(obj_asteroid);
		DrawObject(obj_asteroid);

		if (!IsVisible(obj_asteroid))
			DestroyGameObject(id_asteroid);
	}
}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	// Create the manager
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	CentreAllSpriteOrigins();
	// Load background to be drawn later
	LoadBackground("Data\\Backgrounds\\background.png");
	PLAYER = CreateGameObject(TYPE_PLAYER, { 225, 150 }, 40, "playerChar");
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	++currentTime;
	++asteroidCurrent;
	DrawBackground();
	HandlePlayerControls();
	SpawnAsteroids();
	UpdateAsteroids();
	UpdateShots();
	UpdateDestroyed();
	gamestate.time += elapsedTime;
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

