#pragma once
#include "Entity.h"
#include "FlareMap.h"
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
//#define level1FILE "map01.txt"
#define level1FILE "mapA.txt"
#define level2FILE "map02.txt"
#define level3FILE "map03.txt"
#define Solids {121, 122, 123, 124, 125, 130, 152, 155, 162}
enum GameMode { Menu, Instruction, Quit, Level1, Level2, Level3, Victory, Defeat };

class GameState {
public:
	int TextureID;
	FlareMap map1;
	FlareMap map2;
	FlareMap map3;
	// TODO: set the default to be Menu once we have a menu
	GameMode mode = Level1;

	std::vector<Entity> entities;
	std::unordered_set<int> solidTiles;
	Entity player;

	// Emulates keydown event, keys is faster than polling
	bool canJump = true;

	bool playerHasKey = false;
	//stores the key's x,y tile coord after picked up
	int keyX, keyY;
	//stores the coordinates of the exit
	int doorX, doorY;

	Matrix viewMatrix;

	Mix_Chunk *ghost;
	Mix_Chunk *jump;
	Mix_Music *bgm;
	Mix_Chunk *keyPickUp;
	Mix_Chunk *doorLock;
	Mix_Chunk *doorOpen;
	Mix_Chunk *splash;

	~GameState();
	void loadResources();
	FlareMap& chooseMap();
	void GameState::setExitCoordinates(const FlareMap& map);
	void goToNextLevel();
	void playerDeath();
	void pickUpKey(int gridY, int gridX);
	void updateGameState(float elapsed);
	void updateLevel(float elapsed);
	void processKeys(const Uint8 *keys);
	void processKeysInLevel(const Uint8 *keys);
	bool checkEntityOutOfBounds(const Entity& other);
	void PlaceEntity(std::string type, float x, float y);
	void Render(ShaderProgram &program);
	void GameState::playBackgroundMusic() const;
};