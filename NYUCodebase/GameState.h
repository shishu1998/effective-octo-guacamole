#pragma once
#include "MovingPlatform.h"
#include "FlareMap.h"
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
#define menuFILE "menuMap.txt"
#define level1FILE "mapA.txt"
#define level2FILE "mapB.txt"
#define level3FILE "mapC.txt"
#define Solids {62, 63, 64, 65, 92, 95, 121, 122, 123, 124, 125, 130, 152, 155, 162, 182, 183, 184, 185, 212, 215, 222}
#define Fluids {11, 40, 13, 42}
enum GameMode { Menu, Instruction, Quit, Level1, Level2, Level3, Victory, Defeat };

class GameState {
public:
	int TextureID;
	int fontTextureID;
	FlareMap menuMap;
	FlareMap map1;
	FlareMap map2;
	FlareMap map3;
	GameMode mode = Menu;
	bool finished = false;
	bool playerIsHigh = false;

	std::vector<Entity> enemies;
	std::vector<Entity> boxes;
	std::vector<MovingPlatform> platforms;
	std::unordered_set<int> solidTiles;
	std::unordered_set<int> fluidTiles;
	Entity player;
	Entity playerLife;

	// Emulates keydown event, keys is faster than polling
	bool canJump = true;

	//Player Health, Lives, and Invulnerability stuff
	int lives = 3;
	int playerHealth = 3;
	float invulTime = 0;
	bool playerHasDied = false;
	std::vector<Entity> healthSprites;

	bool playerHasKey = false;
	//stores the key's x,y tile coord after picked up
	int keyX, keyY;
	//stores the coordinates of the exit
	int doorX, doorY;
	// Mushroom Stuff
	Entity mushroomTile;
	Entity mushroom;
	float mushroomElapsed = 0;

	Matrix viewMatrix;
	float animationElapsed;

	Mix_Music *menuMusic;
	Mix_Music *L1Music;
	Mix_Music *L2Music;
	Mix_Music *L3Music;
	Mix_Music *victoryMusic;
	Mix_Music *gameOverMusic;
	Mix_Music *bgm;
	Mix_Chunk *ghost;
	Mix_Chunk *jump;
	Mix_Chunk *keyPickUp;
	Mix_Chunk *doorLock;
	Mix_Chunk *doorOpen;
	Mix_Chunk *splash;
	Mix_Chunk *lava;

	~GameState();

	void loadResources();
	FlareMap& chooseMap();
	void GameState::setExitCoordinates(const FlareMap& map);
	void setupLevel();
	void goToNextLevel();
	void setupHealth();
	void resetEntities();
	void playerDeath();
	void pickUpKey(int gridY, int gridX);
	void resetKey();

	void updateGameState(float elapsed);
	void updateLevel(float elapsed);
	void processKeys(const Uint8 *keys);
	void processKeysInLevel(const Uint8 *keys);
	void processEvents(SDL_Event &event);

	bool checkEntityOutOfBounds(const Entity& other);
	void PlaceEntity(std::string type, float x, float y);

	void GameState::RenderLevelIntro(ShaderProgram& program);
	void Render(ShaderProgram &program);

	void GameState::playBackgroundMusic() const;
};