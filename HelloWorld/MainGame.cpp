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

// Game state
struct GameState {
	int score = 0;
	float time = 0;
};

// All types of objects in the game
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

// Spawn an asteroid at coordinates X, Y given by the function, with a velocity of X, Y to move to that direction
void SpawnAsteroid(int spawnX, int spawnY, int velocityX, int velocityY) {
	Vector2D spawnPos = { spawnX, spawnY }; // Create spawn position

	int id = CreateGameObject(TYPE_ASTEROID, spawnPos, 10, "asteroid1"); // Create the game object
	GetGameObject(id).velocity = { velocityX, velocityY }; // Give the game object it's direction
}

// Spawn a miniature asteroid at coordinates X, Y given by the function, with a velocity of X, Y to move to that direction
void SpawnMiniAsteroid(int spawnX, int spawnY, int amount) {

	for (int i = 0; i <= amount; i++) { // Loop through for "amount" times
		int asteroidVeloX = RandomRollRange(-1, 1); // Get random X velocity
		int asteroidVeloY = RandomRollRange(-1, 1); // Get random Y velocity

		if ((asteroidVeloX != 0) && (asteroidVeloY != 0)) { // Check if the velocity isn't 0,0 (still)
			Vector2D spawnPos = { spawnX, spawnY }; // Create spawn position

			int id = CreateGameObject(TYPE_SMALL_ASTEROID, spawnPos, 3, "asteroid2"); // Create game object
			GetGameObject(id).velocity = { asteroidVeloX, asteroidVeloY }; // Give the game object it's direction
		}
	}
}

// Similar to the function above, spawning a miniature asteroid but with an already pre-defined Vector2D position
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

// Spawn particles at coordatines X, Y given by the function, with the velocity of X, Y to move to that direction
void SpawnParticles(int spawnX, int spawnY, int amount) {
	Vector2D spawnPos = { spawnX, spawnY }; // Create the spawning position

	for (int i = 0; i <= amount; i++) { // Loop for the amount of particles
		int particleVeloX = RandomRollRange(-1, 1); // Get random X velocity
		int particleVeloY = RandomRollRange(-1, 1); // Get random Y velocity

		if ((particleVeloX != 0) && (particleVeloY != 0)) { // Check if the velocity isn't 0,0 (still)
			int id = CreateGameObject(TYPE_PARTICLE, spawnPos, 1, "bullet"); // Create the game object
			GetGameObject(id).velocity = { particleVeloX, particleVeloY }; // Give the game object it's direction
		}
	}
}

// Similar to the function above, spawning a particle asteroid with an already pre-defined Vector2D position
void SpawnParticles(Vector2D pos, int amount) {
	for (int i = 0; i <= amount; i++) { // Loop through the amount of particles wanted
		int particleVeloX = RandomRollRange(-1, 1); // Generate random X velocity
		int particleVeloY = RandomRollRange(-1, 1); // Generate random Y velocity 

		if ((particleVeloX != 0) && (particleVeloY != 0)) { // Check if the velocity isn't 0,0 (still)
			int id = CreateGameObject(TYPE_PARTICLE, pos, 1, "bullet"); // Create the game object
			GetGameObject(id).velocity = { particleVeloX, particleVeloY }; // Give the game object it's direction
		}
	}
}

void HandlePlayerControls() { // Handle all player controls

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

	if (KeyDown(VK_SPACE) || KeyDown(VK_LBUTTON)) { // Check if input is either Spacebar or left click

		Vector2D firePos = obj_player.pos; // Get the fire position, which is the player's position
		int id = CreateGameObject(TYPE_BULLET, firePos, 2, "bullet"); // Create the bullet game object
		GameObject& bul = GetGameObject(id); // instantiate a game object variable
		PointGameObject(bul, 3, mousePos.x, mousePos.y); // Point the bullet in the direction of the mouse's current coordinates
		
	}
	
	UpdateGameObject(obj_player); // Update the player game object
	DrawObjectRotated(obj_player); // Draw the player game object
	PointGameObject(obj_player, 0, mousePos.x, mousePos.y); // Point the player game object in the direction of the mouse
}

void UpdateShots() {
	std::vector<int> shots = CollectGameObjectIDsByType(TYPE_BULLET); // Get all bullets
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID); // Get all asteroids
	std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID); // Get all small asteroids

	for (int id_shot : shots) { // For all bullets, loop

		GameObject& obj_shot = GetGameObject(id_shot); // Instantiate a bullet game object
		bool hasCollided = false; // If the object has collided or not boolean
		int amount = RandomRollRange(2, 6); // Get a random amount (small asteroids)
		int particleAmount = RandomRollRange(3, 7); // Get a random amount (Particles)
		
		for (int id_asteroid : asteroids) { // Loop all asteroids
			GameObject& obj_asteroid = GetGameObject(id_asteroid); // instantiate an asteroid game object
			if (IsColliding(obj_shot, obj_asteroid)) { // Check if the shot and asteroid are colliding
				hasCollided = true; // If so, check to true
				obj_asteroid.type = TYPE_DESTROYED; // Change asteroid's type to "Destroyed"
				gamestate.score += 100; // Add score to the game's scoreboard

				SpawnMiniAsteroid(obj_asteroid.pos, amount); // Spawn mini asteroid function 
				SpawnParticles(obj_asteroid.pos, particleAmount); // Spawn particles function
			}
		}

		for (int id_smallAsteroid : smallAsteroids) { // Loop all small asteroids
			GameObject& obj_ast = GetGameObject(id_smallAsteroid); // instantiate a small asteroid game object
			if (IsColliding(obj_shot, obj_ast)) { // Check if shot is colliding with smaller asteroid
				hasCollided = true; // If so, check to true
				obj_ast.type = TYPE_DESTROYED; // Add small asteroid to type "destroyed"
				gamestate.score += 50; // Add to game score

				SpawnParticles(obj_ast.pos, particleAmount); // Spawn particles function
			}
		}

		UpdateGameObject(obj_shot); // Update the shot's game object (destroyed or not)
		DrawObject(obj_shot); // Draw the shot

		if (!IsVisible(obj_shot) || hasCollided) // If the shot isn't visible, and has collided
			DestroyGameObject(id_shot); // Destroy the bullet
	}
}

void UpdateDestroyed() { // Update the currently "dead" game objects
	std::vector<int> dead = CollectGameObjectIDsByType(TYPE_DESTROYED); // Get all objects of type "destroyed"

	for (int id_dead : dead) { // Loop through all destroyed objects
		GameObject& obj_dead = GetGameObject(id_dead); // Instantiate "dead" game object
		obj_dead.animSpeed = 0.2f; // Animation speed
		UpdateGameObject(obj_dead); // Update the "dead" game object

		//if (obj_dead.frame % 2)
		//	DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);

		if (!IsVisible(obj_dead) || obj_dead.frame >= 10) // If the dead object is on screen or not
			DestroyGameObject(id_dead); // Kill object
	}
}

void SpawnAsteroids() { // Randomly spawn asteroids around the screen
	if (asteroidCurrent - asteroidStart > asteroidDelay) { // Check if the delay is more or less than the current time
		asteroidStart = asteroidCurrent; // Set the current time to the new start time
		asteroidCurrent = 0; // set the current time to 0

		int where = RandomRollRange(0, 4); // Random number between 0 and 4 (edges of the screen)

		if (where == 0) { // Left side of the screen
			// left
			// {0, 1-300}
			SpawnAsteroid(0, RandomRollRange(1, 300), 1, 0); // Spawn asteroid function
		}
		else if (where == 1) { // Top edge of the screen
			// top
			// {1-450, 0}
			SpawnAsteroid(RandomRollRange(1, 450), 0, 0, 1); // Spawn asteroid function
		}
		else if (where == 2) { // Right side of the screen
			// right
			// {450, 1-300}
			SpawnAsteroid(450, RandomRollRange(1, 300), -1, 0); // Spawn asteroid function
		}
		else { // Bottom and wherever else of the screen
			// bottom
			// {1-450, 300}
			SpawnAsteroid(RandomRollRange(1, 450), 300, 0, -1); // Spawn asteroid function
		}
	}
}

void UpdateAsteroids() { // Upddate currently created asteroids
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID); // Get all asteroids

	for (int id_asteroid : asteroids) { // Loop through asteroidsd
		GameObject& obj_asteroid = GetGameObject(id_asteroid); // Instantiate a game object variable for the asteroid

		UpdateGameObject(obj_asteroid); // Update the asteroid
		DrawObject(obj_asteroid); // Draw the asteroid post update

		if (!IsVisible(obj_asteroid)) // If you can't see the asteroid, kill it
			DestroyGameObject(id_asteroid);
	}
}

void UpdateParticles() { // Update currently created particles
	std::vector<int> particles = CollectGameObjectIDsByType(TYPE_PARTICLE); // Get all particles

	for (int id_particle : particles) { // Loop through particles
		GameObject& obj_particle = GetGameObject(id_particle); // Instantiate the particle game object

		UpdateGameObject(obj_particle); // Update particle
		DrawObject(obj_particle); // Draw particle post update

		if (!IsVisible(obj_particle)) // If you can't see the particle, kill it
			DestroyGameObject(id_particle);
	}
}

void UpdateMiniAsteroids() { // Update currently created mini asteroids
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID); // Get all mini asteroids

	for (int id_asteroid : asteroids) { // Loop through mini asteroids
		GameObject& obj_asteroid = GetGameObject(id_asteroid); // Instantiate the mini asteroid game object

		UpdateGameObject(obj_asteroid); // Update mini asteroid
		DrawObject(obj_asteroid); // Draw mini asteroid

		if (!IsVisible(obj_asteroid)) // If you can't see the mini asteroid, kill it
			DestroyGameObject(id_asteroid);
	}
}

void UpdatePlayer() { // Update the current player
	std::vector<int> players = CollectGameObjectIDsByType(TYPE_PLAYER); // Get all players, should return to just 1
	std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID); // Get all asteroids
	std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID); // Get all small asteroids

	for (int id_player : players) { // Loop through players (Should be just once)
		GameObject& obj_player = GetGameObject(id_player); // Instantiate player's game object

		for (int id_asteroid : asteroids) { // Loop through asteroids
			GameObject& obj_asteroid = GetGameObject(id_asteroid); // Instantiate asteroid game object
			if (IsColliding(obj_player, obj_asteroid)) { // Check if player and asteroid are colliding
				gameOver = true; // If so, game is over
				DestroyGameObject(id_asteroid); // Destroy the asteroid
			}
		}

		for (int id_small_asteroid : smallAsteroids) { // Loop through mini asteroids
			GameObject& obj_small_asteroid = GetGameObject(id_small_asteroid); // Instantiate the mini asteroid game object
			if (IsColliding(obj_player, obj_small_asteroid)) { // Check if player and mini asteroid are colliding
				gameOver = true; // If so, game is over
				DestroyGameObject(id_small_asteroid); // Destroy the mini asteroid
			}
		}
	}
}

// Clear game function
void ResetGame() {
	if (gameOver) {
		std::vector<int> players = CollectGameObjectIDsByType(TYPE_PLAYER);
		std::vector<int> asteroids = CollectGameObjectIDsByType(TYPE_ASTEROID);
		std::vector<int> smallAsteroids = CollectGameObjectIDsByType(TYPE_SMALL_ASTEROID);
		std::vector<int> destroyed = CollectGameObjectIDsByType(TYPE_DESTROYED);
		std::vector<int> particles = CollectGameObjectIDsByType(TYPE_PARTICLE);

		for (int player : players)
			DestroyGameObject(player);

		for (int asteroid : asteroids)
			DestroyGameObject(asteroid);

		for (int asteroid : smallAsteroids)
			DestroyGameObject(asteroid);

		for (int d : destroyed)
			DestroyGameObject(d);

		for (int particle : particles)
			DestroyGameObject(particle);
	}
}

// "Expression must have a class" -> Error
//void DrawScore(int score) {
//	char* char_new = new char[gamestate.score.length()];
//	DrawDebugText({ 40, 15 }, , cWhite);
//}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ) // Similar to Unity's "onAwake()"
{
	// Create the manager
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE ); 
	CentreAllSpriteOrigins();
	// Load backgrounds to be drawn later
	LoadBackground("Data\\Backgrounds\\background.png"); // Load Game's black background
	LoadBackground("Data\\Backgrounds\\splashScreen.png"); // load "Asteroids" background
	LoadBackground("Data\\Backgrounds\\GameOver.png"); // Load the game over background
	LoadBackground("Data\\Backgrounds\\PlayBackground.png"); // Load the main menu background
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	++splashCurrent; // Splash screen ticker

	if (splashCurrent > splashDelay) { // Check if X seconds has passed
		if (!playPressed) { // Check if "Play" has been pressed
			DrawBackground(3); // If not, draw the main menu
			if (KeyDown(VK_SPACE)) { // Space = play
				playPressed = true; // Game has started
			}
		}
		else {
			if (!gameOver) { // If the game's not over
				++asteroidCurrent; // The asteroid ticker
				PLAYER = CreateGameObject(TYPE_PLAYER, { 225, 150 }, 15, "playerChar"); // Create the player character
				DrawBackground(0); // Draw the game background
				HandlePlayerControls(); // Handle all player controls
				SpawnAsteroids(); // Spawn random asteroids
				UpdateAsteroids(); // Update the asteroids
				UpdateShots(); // Update bullets
				UpdateDestroyed(); // Update currently destroyed game objects
				UpdateParticles(); // Update particle effects
				UpdateMiniAsteroids(); // Update mini asteroids
				UpdatePlayer(); // Update the player
			}
			else {
				DrawBackground(2); // Show the "Game over" screen
				ResetGame(); // Reset the game fully
				if (KeyDown(VK_SPACE)) { // Space = start over again
					gameOver = false; // Game is no longer over
					gamestate.score = 0; // reset score
				}
				else if (KeyDown(VK_ESCAPE)) { // Escape = Game closes
					return KeyDown(VK_ESCAPE);
				}
			}
			
		}
	}
	else {
		DrawBackground(1); // Draw splash screen
	}

	//DrawScore(gamestate.score);
	gamestate.time += elapsedTime; // Game timer
	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

