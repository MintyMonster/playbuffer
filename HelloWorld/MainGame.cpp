#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

int DISPLAY_WIDTH = 250;
int DISPLAY_HEIGHT = 277;
int DISPLAY_SCALE = 2;

using namespace Play;

struct GameState {
	float timer = 0;
	int spriteId = 0;
	int points = 0;
};

GameState gameState;

enum GameObjectType {
	TYPE_NULL = -1,
	TYPE_PACMAN,
};

void HandlePlayerControls() {
	//CentreAllSpriteOrigins();
	//CreateGameObject(TYPE_PACMAN, { 100, 100 }, 10, "pacman");

	GameObject& obj_pacman = GetGameObjectByType(TYPE_PACMAN);
	if (KeyDown(VK_UP)) {
		obj_pacman.velocity = { 0, -4 };
		SetSprite(obj_pacman, "pacmanup", 0.25f);
	} else if (KeyDown(VK_DOWN)) {
		obj_pacman.velocity = { 0, 4 };
		SetSprite(obj_pacman, "pacmandown", 0.25f);
	} else if (KeyDown(VK_LEFT)) {
		obj_pacman.velocity = { -4, 0 };
		SetSprite(obj_pacman, "pacmanleft", 0.25f);
	} else if (KeyDown(VK_RIGHT)) {
		obj_pacman.velocity = { 4, 0 };
		SetSprite(obj_pacman, "pacman", 0.25f);
	}

	UpdateGameObject(obj_pacman);

}

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	// Load pacman background
	LoadBackground("Data\\Backgrounds\\pacmanbackground.png");
	Play::CreateGameObject(TYPE_PACMAN, { 100, 100 }, 50, "pacman");
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate( float elapsedTime )
{
	gameState.timer += elapsedTime;
	//Play::ClearDrawingBuffer( Play::cBlack );
	// Draw background onto plain
	DrawBackground();
	//DrawSpriteRotated("pacman", { 100,100 }, 0, 0, 0.9, 1);
	HandlePlayerControls();

	Play::PresentDrawingBuffer();
	return Play::KeyDown( VK_ESCAPE );
}

// Gets called once when the player quits the game 
int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

