#pragma once
#include "Entity.h"
#include "FlareMap.h"
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
#define level1FILE "map01.txt"
#define level2FILE "map02.txt"
#define level3FILE "map03.txt"
#define Solids {121, 122, 123, 124, 125, 130, 152, 155}
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

	// FIXUP: Use Event Polling instead of a variable in the gamestate
	bool canJump = true;

	bool playerHasKey = false;
	//stores the key's x,y tile coord after picked up
	int keyX, keyY;

	Vector4 start;
	Matrix viewMatrix;

	Mix_Chunk *ghost;
	Mix_Chunk *jump;
	Mix_Music *bgm;
	Mix_Chunk *keyPickUp;
	Mix_Chunk *doorLock;
	Mix_Chunk *doorOpen;

	~GameState();
	void loadResources();
	FlareMap& chooseMap();
	void goToNextLevel();
	void initEntities();
	void reset();
	void resetPlayerPosition();
	void pickUpKey(int gridY, int gridX);
	void updateGameState(float elapsed);
	void processKeys(const Uint8 *keys);
	void PlaceEntity(std::string type, float x, float y);
	void Render(ShaderProgram &program);
	void GameState::playBackgroundMusic() const;
};