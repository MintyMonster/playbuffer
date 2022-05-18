#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <charconv>

#define MAX_DIGITS 15

int DISPLAY_WIDTH = 450;
int DISPLAY_HEIGHT = 300;
int DISPLAY_SCALE = 2;
int PLAYER;
int ASTEROID;
int SMALL_ASTEROID;
int SPACESHIP;
int angle = 1; // 1 = up, 2 = down, 3 = left, 4 = right;

// Player shooting timer
//float startTime = 0;
//float currentTime = 0;
//float delay = 0.05f;

// Asteroid spawn timer
float asteroidStart = 0;
float asteroidCurrent = 0;
float asteroidDelay = 1.0f;

// Splash screen timer
float splashCurrent = 0;
float splashDelay = 350.0f;

// Pressed Play
bool playPressed = false;
// Win or Loss
bool gameOver = false;

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
	TYPE_PARTICLE,
};

GameState gamestate;

void SpawnAsteroid(int spawnX, int spawnY, int velocityX, int velocityY) {
	Vector2D spawnPos = { spawnX, spawnY };

	int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 15, "asteroid1");
	GetGameObject(id).velocity = { velocityX, velocityY };
}

void SpawnMiniAsteroid(int spawnX, int spawnY, int amount) {

	for (int i = 0; i <= amount; i++) {
		int asteroidVeloX = RandomRollRange(-1, 1);
		int asteroidVeloY = RandomRollRange(-1, 1);

		if ((asteroidVeloX != 0) && (asteroidVeloY != 0)) {
			Vector2D spawnPos = { spawnX, spawnY };

			int id = CreateGameObject(TYPE_SMALL_ASTEROID, spawnPos, 3, "asteroid2");
			GetGameObject(id).velocity = { asteroidVeloX, asteroidVeloY };
		}
	}
}

void SpawnMiniAsteroid(Vector2D pos, int amount) {

	for (int i = 0; i <= amount; i++) {
		int asteroidVeloX = RandomRollRange(-1, 1);
		int asteroidVeloY = RandomRollRange(-1, 1);

		if ((asteroidVeloX != 0) && (asteroidVeloY != 0)) {
			int id = CreateGameObject(TYPE_SMALL_ASTEROID, pos, 3, "asteroid2");
			GetGameObject(id).velocity = { asteroidVeloX, asteroidVeloY };
		}
	}
}

void SpawnParticles(Vector2D pos, int amount) {
	for (int i = 0; i <= amount; i++) {
		int particleVeloX = RandomRollRange(-1, 1);
		int particleVeloY = RandomRollRange(-1, 1);

		if ((particleVeloX != 0) && (particleVeloY != 0)) {
			int id = CreateGameObject(TYPE_PARTICLE, pos, 1, "bullet");
			GetGameObject(id).velocity = { particleVeloX, particleVeloY };
		}
	}
}

void HandlePlayerControls() {

	GameObject& obj_player = GetGameObjectByType(TYPE_PLAYER);
	Point2D mousePos = GetMousePos();

	if (KeyDown(0x57)) { // up
		obj_player.velocity = { 0,-1 };
		//SetSprite(obj_player, "playerChar", 0.2f);
		angle = 1;
	}
	else if (KeyDown(0x53)) { // down
		obj_player.velocity = { 0, 1 };
		//SetSprite(obj_player, "playerChar_down", 0.2f);
		angle = 2;
	}
	else if (KeyDown(0x44)) { // right
		obj_player.velocity = { 1, 0 };
		//SetSprite(obj_player, "playerChar_right", 0.2f);
		angle = 4;
	}
	else if (KeyDown(0x41)) { // left
		obj_player.velocity = { -1, 0 };
		//SetSprite(obj_player, "playerChar_left", 0.2f);
		angle = 3;
	}
	else {
		obj_player.velocity *= 0.5f;
		obj_player.acceleration = { 0, 0 };
	}

	if (KeyDown(VK_SPACE) || KeyDown(VK_LBUTTON)) {
		/*
		if (currentTime - startTime > delay) {
			startTime = currentTime;
			currentTime = 0;

			
		} */

		/*
		if (angle == 1)
			shotAngle = { 0, -3 };
		else if (angle == 2)
			shotAngle = { 0, 3 };
		else if (angle == 3)
			shotAngle = { -3, 0 };
		else if (angle == 4)
			shotAngle = { 3, 0 };
			*/

		Vector2D firePos = obj_player.pos;
		int id = CreateGameObject(TYPE_BULLET, firePos, 2, "bullet");
		GameObject& bul = GetGameObject(id);
		PointGameObject(bul, 3, mousePos.x, mousePos.y);
		
	}
	
	UpdateGameObject(obj_player);
	DrawObjectRotated(obj_player);
	PointGameObject(obj_player, 0, mousePos.x, mousePos.y);
}

void UpdateShots() {
	std::vector<int> shots = CollectGameObjectIDsByType(TYPE_BULLET);
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID);
	std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID);

	for (int id_shot : shots) {

		GameObject& obj_shot = GetGameObject(id_shot);
		bool hasCollided = false;
		int amount = RandomRollRange(2, 6);
		int particleAmount = RandomRollRange(3, 7);
		
		for (int id_asteroid : asteroids) {
			GameObject& obj_asteroid = GetGameObject(id_asteroid);
			if (IsColliding(obj_shot, obj_asteroid)) {
				hasCollided = true;
				obj_asteroid.type = TYPE_DESTROYED;
				gamestate.score += 100;

				SpawnMiniAsteroid(obj_asteroid.pos, amount);
				SpawnParticles(obj_asteroid.pos, particleAmount);
			}
		}

		for (int id_smallAsteroid : smallAsteroids) {
			GameObject& obj_ast = GetGameObject(id_smallAsteroid);
			if (IsColliding(obj_shot, obj_ast)) {
				hasCollided = true;
				obj_ast.type = TYPE_DESTROYED;
				gamestate.score += 100;

				SpawnParticles(obj_ast.pos, particleAmount);
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

		//if (obj_dead.frame % 2)
		//	DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);

		if (!IsVisible(obj_dead) || obj_dead.frame >= 10)
			DestroyGameObject(id_dead);
	}
}

void SpawnAsteroids() {
	if (asteroidCurrent - asteroidStart > asteroidDelay) {
		asteroidStart = asteroidCurrent;
		asteroidCurrent = 0;

		int what = RandomRollRange(1, 3);
		int where = RandomRollRange(0, 4);;
		Vector2D spawnPos;

		if (where == 0) {
			// left
			// {0, 1-300}
			SpawnAsteroid(0, RandomRollRange(1, 300), 1, 0);
		}
		else if (where == 1) {
			// top
			// {1-450, 0}
			SpawnAsteroid(RandomRollRange(1, 450), 0, 0, 1);
		}
		else if (where == 2) {
			// right
			// {450, 1-300}
			SpawnAsteroid(450, RandomRollRange(1, 300), -1, 0);
		}
		else {
			// bottom
			// {1-450, 300}
			SpawnAsteroid(RandomRollRange(1, 450), 300, 0, -1);
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

void UpdateParticles() {
	std::vector<int> particles = CollectGameObjectIDsByType(TYPE_PARTICLE);

	for (int id_particle : particles) {
		GameObject& obj_particle = GetGameObject(id_particle);

		UpdateGameObject(obj_particle);
		DrawObject(obj_particle);

		if (!IsVisible(obj_particle))
			DestroyGameObject(id_particle);
	}
}

void UpdateMiniAsteroids() {
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID);

	for (int id_asteroid : asteroids) {
		GameObject& obj_asteroid = GetGameObject(id_asteroid);

		UpdateGameObject(obj_asteroid);
		DrawObject(obj_asteroid);

		if (!IsVisible(obj_asteroid))
			DestroyGameObject(id_asteroid);
	}
}

void UpdatePlayer() {
	std::vector<int> players = CollectGameObjectIDsByType(TYPE_PLAYER);
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID);
	std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID);

	for (int id_player : players) {
		GameObject& obj_player = GetGameObject(id_player);

		for (int id_asteroid : asteroids) {
			GameObject& obj_asteroid = GetGameObject(id_asteroid);
			if (IsColliding(obj_player, obj_asteroid)) {
				gameOver = true;
				DestroyGameObject(id_asteroid);
			}
		}

		for (int id_small_asteroid : smallAsteroids) {
			GameObject& obj_small_asteroid = GetGameObject(id_small_asteroid);
			if (IsColliding(obj_player, obj_small_asteroid)) {
				gameOver = true;
				DestroyGameObject(id_small_asteroid);
			}
		}
	}
}

// "Expression must have a class" -> Error
//void DrawScore(int score) {
//	char* char_new = new char[gamestate.score.length()];
//	DrawDebugText({ 40, 15 }, , cWhite);
//}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	// Create the manager
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	CentreAllSpriteOrigins();
	// Load backgrounds to be drawn later
	LoadBackground("Data\\Backgrounds\\background.png");
	LoadBackground("Data\\Backgrounds\\splashScreen.png");
	LoadBackground("Data\\Backgrounds\\GameOver.png");
	LoadBackground("Data\\Backgrounds\\PlayBackground.png");
	PLAYER = CreateGameObject(TYPE_PLAYER, { 225, 150 }, 15, "playerChar");
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	++splashCurrent;

	if (splashCurrent > splashDelay) {
		if (!playPressed) {
			DrawBackground(3);
			if (KeyDown(VK_SPACE)) {
				playPressed = true;
			}
		}
		else {
			if (!gameOver) {
				++asteroidCurrent;
				DrawBackground(0);
				HandlePlayerControls();
				SpawnAsteroids();
				UpdateAsteroids();
				UpdateShots();
				UpdateDestroyed();
				UpdateParticles();
				UpdateMiniAsteroids();
				UpdatePlayer();
			}
			else {
				DrawBackground(2);
				if (KeyDown(VK_SPACE)) {
					gameOver = false;
				}
				else if (KeyDown(VK_ESCAPE)) {
					return KeyDown(VK_ESCAPE);
				}
			}
			
		}
	}
	else {
		DrawBackground(1);
	}

	//DrawScore(gamestate.score);
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

