#define _USE_MATH_DEFINES
#include "GameState.h"
#include "Helper.h"

//Loads the required resources for the entities
void GameState::loadResources() {
	TextureID = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");
	map1.Load(level1FILE);
	// TODO: Move this into goToNextLevel once we have a menu
	for (int i = 0; i < map1.entities.size(); i++) {
		PlaceEntity(map1.entities[i].type, map1.entities[i].x * tileSize, map1.entities[i].y * -tileSize);
	}
	setExitCoordinates(map1);
	map2.Load(level2FILE);
	map3.Load(level3FILE);
	solidTiles = std::unordered_set<int>(Solids);
	fluidTiles = std::unordered_set<int>(Fluids);
	viewMatrix.Translate(-player.Position.x, -player.Position.y, 0);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	bgm = Mix_LoadMUS("Running.mp3");
	ghost = Mix_LoadWAV("ghost.wav");
	jump = Mix_LoadWAV("boing_spring.wav");
	keyPickUp = Mix_LoadWAV("coin.wav");
	doorLock = Mix_LoadWAV("doorLock.wav");
	doorOpen = Mix_LoadWAV("doorOpen.wav");
	splash = Mix_LoadWAV("splash.wav");
}

FlareMap & GameState::chooseMap()
{
	switch (mode) {
		case Level1:
			return map1;
		case Level2:
			return map2;
		case Level3:
			return map3;
	}
}

//Scans and sets the coordinates of the door on the map
void GameState::setExitCoordinates(const FlareMap& map) {
	for (int i = 0; i < map.mapData.size(); ++i) {
		for (int j = 0; j < map.mapData[i].size(); ++j) {
			if (map.mapData[i][j] == 167) {
				doorX = j;
				doorY = i;
			}
		}
	}
}

void GameState::goToNextLevel() {
	switch (mode) {
		case Menu:
			mode = Level1;
			break;
		case Level1:
			mode = Level2;
			enemies.clear();
			boxes.clear();
			platforms.clear();
			//player.reset(); //???? orig player pos change
			glClearColor(0.455f, 0.0f, 0.416f, 1.0f);
			for (int i = 0; i < map2.entities.size(); i++) {
				PlaceEntity(map2.entities[i].type, map2.entities[i].x * tileSize, map2.entities[i].y * -tileSize);
			}
			setExitCoordinates(map2);
			break;
		case Level2:
			mode = Level3;
			enemies.clear();
			boxes.clear();
			platforms.clear();
			for (int i = 0; i < map3.entities.size(); i++) {
				PlaceEntity(map3.entities[i].type, map3.entities[i].x * tileSize, map3.entities[i].y * -tileSize);
			}
			setExitCoordinates(map3);
			break;
		case Level3:
			mode = Victory;
			break;
	}
}

//Plays the background music
void GameState::playBackgroundMusic() const{
	Mix_VolumeMusic(20);
	Mix_PlayMusic(bgm, -1);
}

//Resets players and entities upon death
void GameState::playerDeath() {
	player.reset();
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].reset();
		enemies[i].forward = true;
	}
	for (int i = 0; i < boxes.size(); ++i) {
		boxes[i].reset();
	}
	FlareMap& map = chooseMap();
	//put the key back and lock the door if the player already has a key
	if (playerHasKey) {
		map.mapData[keyY][keyX] = 14;
		playerHasKey = false;
		map.mapData[doorY][doorX] = 167;
		map.mapData[doorY-1][doorX] = 166;
	}
}

//Player picks up key
void GameState::pickUpKey(int gridY, int gridX) {
	playerHasKey = true;
	Mix_PlayChannel(-1, keyPickUp, 0);
	//save key's original coord
	keyX = gridX;
	keyY = gridY;
	FlareMap& map = chooseMap();
	//no more key at that location
	map.mapData[gridY][gridX] = 0;
	//set door to "unlocked"
	map.mapData[doorY][doorX] = 137;
	map.mapData[doorY-1][doorX] = 136;
}

//Updates the GameState based on the time elapsed
void GameState::updateGameState(float elapsed) {
	switch (mode) {
	case Level1:
	case Level2:
	case Level3:
		updateLevel(elapsed);
		break;
	}
}

void GameState::updateLevel(float elapsed)
{
	std::pair<float, float> penetration;
	FlareMap& map = chooseMap();
	player.Update(elapsed, map.mapData, solidTiles);
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].Update(elapsed, map.mapData, solidTiles);
		//Enemy jumps if possible 
		if ((enemies[i].canJumpLeft(map.mapData, solidTiles) || enemies[i].canJumpRight(map.mapData, solidTiles)) && enemies[i].collidedBottom) {
			enemies[i].velocity.y = 2.5;
		}
		//Reverses the movement of NPCs if there is collision against tiles or if they can't drop down
		if (enemies[i].collidedLeft || !enemies[i].canDropDownLeft(map.mapData, solidTiles)) {
			enemies[i].acceleration.x = 0.5;
			enemies[i].forward = false;
		}
		if (enemies[i].collidedRight || !enemies[i].canDropDownRight(map.mapData, solidTiles)) {
			enemies[i].acceleration.x = -0.5;
			enemies[i].forward = true;
		}
		//Enemies respawn if goes out of bounds
		if (checkEntityOutOfBounds(enemies[i])) enemies[i].reset();
	}

	//Boxes falling(possibly) update
	for (int i = 0; i < boxes.size(); ++i) {
		boxes[i].Update(elapsed, map.mapData, solidTiles);

		//Player Collision with Box
		player.SATCollidesWith(boxes[i], penetration);
		if (penetration.second > 0 && player.velocity.y < 0){
			player.velocity.y = 0;
			player.collidedBottom = true;
		}
		else if (penetration.second < 0) {
			player.velocity.y = 0;
			player.collidedTop = true;
		}
		//Boxes respawn if fallen out of bounds
		if (checkEntityOutOfBounds(boxes[i])) boxes[i].reset();
	}

	//Platform update, also resolves platform player collision + movement
	for (int i = 0; i < platforms.size(); ++i) {
		platforms[i].Update(elapsed, map.mapData, solidTiles, player);
	}

	//Player restarts when touches an enemy
	for (int i = 0; i < enemies.size(); ++i) {
		if (player.SATCollidesWith(enemies[i], penetration)) {
			playerDeath();
			Mix_PlayChannel(-1, ghost, 0);
		}
	}

	//Player restarts when touches water/lava
	int gridX, gridY;
	worldToTileCoordinates(player.Position.x, player.Position.y, &gridX, &gridY);
	if (fluidTiles.find(map.mapData[gridY][gridX]) != fluidTiles.end()) {
		playerDeath();
		Mix_PlayChannel(-1, splash, 0);
	}
	//Player picks up key on collision
	if (map.mapData[gridY][gridX] == 14) {
		pickUpKey(gridY, gridX);
	}
	//Translate the view matrix by the player's position
	viewMatrix.Identity();
	viewMatrix.Translate(-player.Position.x, -player.Position.y, 0);
}

void GameState::processKeys(const Uint8 * keys)
{
	switch (mode) {
		case Level1:
		case Level2:
		case Level3:
			processKeysInLevel(keys);
			break;
	}
}

//Process the key input while in a level
void GameState::processKeysInLevel(const Uint8 * keys)
{
	if (keys[SDL_SCANCODE_A]) {
		player.acceleration.x = -1.2;
		player.forward = false;
	}
	if (keys[SDL_SCANCODE_D]) {
		player.acceleration.x = 1.2;
		player.forward = true;
	}
	if (keys[SDL_SCANCODE_W]) {
		int gridX, gridY;
		worldToTileCoordinates(player.Position.x, player.Position.y, &gridX, &gridY);
		FlareMap map = chooseMap();
		if (map.mapData[gridY][gridX] == 137 || map.mapData[gridY][gridX] == 138) {
			Mix_PlayChannel(-1, doorOpen, 0);
			playerHasKey = false;
			goToNextLevel();
		}
		else if (map.mapData[gridY][gridX] == 167 || map.mapData[gridY][gridX] == 168) {
			Mix_PlayChannel(-1, doorLock, 0);
		}
	}
	if (keys[SDL_SCANCODE_SPACE] && canJump) {
		if (player.collidedBottom) {
			player.velocity.y = 2.5;
			Mix_PlayChannel(-1, jump, 0);
			canJump = false;
		}
		if (player.collidedLeft || player.collidedRight) {
			player.velocity.y = 1.8;
			player.velocity.x = player.forward ? -1.5 : 1.5;
			canJump = false;
		}
	}
	if (!(keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D])) {
		player.acceleration.x = 0.0;
		player.acceleration.y = 0.0;
	}
	if (!keys[SDL_SCANCODE_SPACE]) canJump = true;
}

//Checks if an entity fell out of the map
bool GameState::checkEntityOutOfBounds(const Entity & other)
{
	FlareMap map = chooseMap();
	int gridTop, gridBottom, gridLeft, gridRight;
	worldToTileCoordinates(other.Position.x - other.size.x / 2, other.Position.y - other.size.y / 2, &gridLeft, &gridBottom);
	worldToTileCoordinates(other.Position.x + other.size.x / 2, other.Position.y + other.size.y / 2, &gridRight, &gridTop);
	return (gridLeft < 0) || (gridRight >= map.mapData[0].size()) || (gridBottom >= map.mapData.size()) || (gridTop < 0);
}

//Creates entities based on the string type
void GameState::PlaceEntity(std::string type, float x, float y)
{
	if (type == "Player") {
		player = Entity(x, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 109, tileSize), createSheetSpriteBySpriteIndex(TextureID, 119, tileSize), createSheetSpriteBySpriteIndex(TextureID, 118, tileSize) }), Player, false);
		player.originalPosition = player.Position;
		player.originalVelocity = player.velocity;
		player.originalAcceleration = player.acceleration;
	}
	else if (type == "Enemy") {
		Entity enemy = Entity(x, y, std::vector<SheetSprite>({createSheetSpriteBySpriteIndex(TextureID, 445, tileSize) }), Enemy, false);
		enemy.acceleration.x = -0.5;
		enemy.originalPosition = enemy.Position;
		enemy.originalVelocity = enemy.velocity;
		enemy.originalAcceleration = enemy.acceleration;
		enemy.forward = true;
		enemies.emplace_back(enemy);
	}
	else if (type == "Moving") {
		MovingPlatform plat = MovingPlatform(TextureID, x, y, 3);
		plat.acceleration.x = 0.3;
		platforms.emplace_back(plat);
	}
	else if (type == "Box") {
		Entity box = Entity(x, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 191, tileSize) }), Box, false);
		box.originalPosition = box.Position;
		boxes.emplace_back(box);
	}
}

//Draws the game state (tilemap and entities)
void GameState::Render(ShaderProgram & program)
{
	switch (mode) {
		case Level1:
		case Level2:
		case Level3:
			DrawLevel(program, TextureID, chooseMap(), viewMatrix, 0.0, 0.0);
			player.Render(program, viewMatrix);
			for (int i = 0; i < enemies.size(); ++i) {
				enemies[i].Render(program, viewMatrix);
			}
			for (int i = 0; i < boxes.size(); ++i) {
				boxes[i].Render(program, viewMatrix);
			}
			for (int i = 0; i < platforms.size(); ++i) {
				platforms[i].Render(program, viewMatrix);
			}
			break;
		}
}

// Destructor for GameState
GameState::~GameState() {
	Mix_FreeChunk(ghost);
	Mix_FreeChunk(jump);
	Mix_FreeMusic(bgm);
	Mix_FreeChunk(keyPickUp);
	Mix_FreeChunk(doorLock);
	Mix_FreeChunk(doorOpen);
	Mix_FreeChunk(splash);
	Mix_CloseAudio();
}
