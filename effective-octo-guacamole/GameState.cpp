#define _USE_MATH_DEFINES
#include "GameState.h"
#include "Helper.h"

//Loads all the music files for the game
void GameState::loadMusic()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	bgm = Mix_LoadMUS("Running.mp3");
	menuMusic = Mix_LoadMUS("MenuMusic.mp3");
	L1Music = Mix_LoadMUS("Level1.mp3");
	L2Music = Mix_LoadMUS("Level2.mp3");
	L3Music = Mix_LoadMUS("Level3.mp3");
	victoryMusic = Mix_LoadMUS("Victory.mp3");
	gameOverMusic = Mix_LoadMUS("GameOver.mp3");
	ghost = Mix_LoadWAV("ghost.wav");
	jump = Mix_LoadWAV("boing_spring.wav");
	keyPickUp = Mix_LoadWAV("coin.wav");
	doorLock = Mix_LoadWAV("doorLock.wav");
	doorOpen = Mix_LoadWAV("doorOpen.wav");
	splash = Mix_LoadWAV("splash.wav");
	lava = Mix_LoadWAV("Lava.wav");
}

//Loads the required resources for the entities
void GameState::loadResources() {
	TextureID = LoadTexture(RESOURCE_FOLDER"spritesheet_rgba.png");
	fontTextureID = LoadTexture(RESOURCE_FOLDER"font1.png");
	menuMap.Load(menuFILE);
	map1.Load(level1FILE);
	map2.Load(level2FILE);
	map3.Load(level3FILE);
	player = Player(0, 0, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 109, tileSize), createSheetSpriteBySpriteIndex(TextureID, 119, tileSize), createSheetSpriteBySpriteIndex(TextureID, 118, tileSize) }));

	for (int i = 0; i < 3; ++i) {
		Entity health = Entity(-16 + i, 9.0, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 373, tileSize), createSheetSpriteBySpriteIndex(TextureID, 375, tileSize) }), false);
		health.size = Vector4(1.0,1.0,1.0);
		health.setResetProperties();
		healthSprites.emplace_back(health);
	}
	playerLife = Entity(-0.3, -0.03, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 172, tileSize)}), false);

	solidTiles = std::unordered_set<int>(Solids);
	loadMusic();
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
		case Menu:
		case Instruction:
			return menuMap;
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

void GameState::setupLevel() {
	FlareMap& map = chooseMap();
	enemies.clear();
	boxes.clear();
	platforms.clear();
	animationElapsed = 0;
	for (int i = 0; i < map.entities.size(); i++) {
		PlaceEntity(map.entities[i].type, map.entities[i].x * tileSize, map.entities[i].y * -tileSize);
	}
	setExitCoordinates(map);
}

void GameState::goToNextLevel() {
	playerHasDied = false;
	cheat = false;
	player.isStatic = false;
	playerIsHigh = false;
	mushroomElapsed = 0.0f;
	switch (mode) {
		case Menu:
			//Resets the player's lives and hp after every playthrough
			player.lives = 3;
			player.health = 3;
			for (int i = 0; i < healthSprites.size(); ++i) {
				healthSprites[i].reset();
			}
			mode = Level1;
			setupHealth();
			setupLevel();
			glClearColor(0.553f, 0.765f, 0.855f, 0.0f);
			playBackgroundMusic();
			break;
		case Level1:
			mode = Level2;
			glClearColor(0.455f, 0.0f, 0.416f, 1.0f);
			setupLevel();
			playBackgroundMusic();
			break;
		case Level2:
			mode = Level3;
			glClearColor(0.043f, 0.29f, 0.494f, 1.0f);
			setupLevel();
			playBackgroundMusic();
			break;
		case Level3:
			mode = Victory;
			playBackgroundMusic();
			break;
		case Victory:
			mode = Menu;
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			playBackgroundMusic();
			break;
		case Defeat:
			mode = Menu;
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			playBackgroundMusic();
			break;
		case Instruction:
			mode = Menu;
			break;
	}
}

void GameState::setupHealth()
{
	for (int i = 0; i < healthSprites.size(); ++i) {
		healthSprites[i].parent = &player;
	}
}

//Plays the background music
void GameState::playBackgroundMusic() const{
	Mix_VolumeMusic(20);
	Mix_HaltMusic();
	switch (mode) {
		case Menu:
			Mix_PlayMusic(menuMusic, -1);
			break;
		case Level1:
			Mix_PlayMusic(L1Music, -1);
			break;
		case Level2:
			Mix_PlayMusic(L2Music, -1);
			break;
		case Level3:
			Mix_PlayMusic(L3Music, -1);
			break;
		case Victory:
			Mix_PlayMusic(victoryMusic, -1);
			break;
		case Defeat:
			Mix_PlayMusic(gameOverMusic, -1);
			break;
		}
}

//Resets the states of all entities in the GameState
void GameState::resetEntities()
{
	player.reset();
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].reset();
		enemies[i].forward = true;
	}
	for (int i = 0; i < boxes.size(); ++i) {
		boxes[i].reset();
	}
	for (int i = 0; i < platforms.size(); ++i) {
		platforms[i].reset();
	}
	for (int i = 0; i < healthSprites.size(); ++i) {
		healthSprites[i].reset();
	}
	mushroomTile.reset();
	mushroom.reset();
	mushroom.alpha = 0.0f;
}

//Resets players and entities upon death
void GameState::playerDeath() {
	player.health = 3;
	invulTime = 0;
	animationElapsed = 0;
	playerHasDied = true;
	playerIsHigh = false;
	cheat = false;
	player.isStatic = false;
	mushroomElapsed = 0;
	resetEntities();
	FlareMap& map = chooseMap();
	//put the key back and lock the door if the player already has a key
	if (playerHasKey) {
		resetKey();
	}
	player.lives -= 1;
	//player has no lives left; game over
	if (!player.lives) {
		mode = Defeat;
		playBackgroundMusic();
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

//Resets original key and door sprites
void GameState::resetKey() {
	FlareMap& map = chooseMap();
	map.mapData[keyY][keyX] = 14;
	map.mapData[doorY][doorX] = 167;
	map.mapData[doorY - 1][doorX] = 166;
	playerHasKey = false;
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
	animationElapsed += elapsed;
	//We don't want players to move + do anything until everything's rendered
	if (animationElapsed > 2.5) {
		std::pair<float, float> penetration;
		FlareMap& map = chooseMap();

		//Invulnerability update
		invulTime -= elapsed;
		float playerAlpha;
		if (invulTime > 0.75) {
			playerAlpha = mapValue(invulTime, 0.75, 1.5, 0.0, 1.0);
			player.alpha = easeOut(playerAlpha, 0.0, elapsed);
		}
		else if (invulTime > 0.0) {
			playerAlpha = mapValue(invulTime, 0.75, 0.0, 0.0, 1.0);
			player.alpha = easeIn(playerAlpha, 1.0, elapsed);
		}
		else {
			invulTime = 0;
			player.alpha = 1.0;
		}

		player.Update(elapsed, map.mapData, solidTiles);
		//This shouldn't ever happen, but just in case
		if (checkEntityOutOfBounds(player)) player.reset();
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
			if (penetration.second > 0 && player.velocity.y < 0) {
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

		//Player collision with MushroomTile
		if (player.SATCollidesWith(mushroomTile, penetration) && penetration.second != 0.0f) {
			if (penetration.second < 0.0f) {
				//Only update mushroom when we haven't popped out a mushroom
				if (mushroomTile.spriteIndex == 0 && player.velocity.y > 0) {
					mushroomTile.spriteIndex = 1;
					mushroom.alpha = 1.0f;
					mushroomElapsed += elapsed;
				}
				player.velocity.y = 0.0f;
			}
			if (penetration.second > 0.0f && player.velocity.y < 0.0f) {
				player.collidedBottom = true;
				player.velocity.y = 0.0f;
			}
		}

		//Updates the mushroom coordinates
		if (mushroomElapsed > 0.0f) {
			float displacementY = easeOut(mushroomElapsed*0.4, 0.0, tileSize/2);
			displacementY = displacementY > tileSize/2 ? tileSize/2 : displacementY;
			mushroom.Position.y = mushroom.originalPosition.y + displacementY;
			mushroomElapsed += elapsed;
		}

		//If player eats a mushroom
		if (mushroom.alpha != 0 && player.SATCollidesWith(mushroom, penetration)) {
			//50-50 chance of getting high or dying
			if (rand() % 2) {
				playerIsHigh = true;
				mushroom.alpha = 0.0f;
			}
			else {
				playerDeath();
			}
		}

		//Platform update, also resolves platform player collision + movement
		for (int i = 0; i < platforms.size(); ++i) {
			platforms[i].Update(elapsed, map.mapData, solidTiles, player);
		}

		//Player loses health when touches an enemy
		for (int i = 0; i < enemies.size(); ++i) {
			if (invulTime <= 0 && player.SATCollidesWith(enemies[i], penetration)) {
				player.health -= 1;
				healthSprites[player.health].spriteIndex += 1;
				invulTime = 1.5;
				Mix_PlayChannel(-1, ghost, 0);
			}
		}
		if (player.health < 1) playerDeath();

		//Player restarts when touches water
		int gridX, gridY;
		worldToTileCoordinates(player.Position.x, player.Position.y, &gridX, &gridY);
		if (map.mapData[gridY][gridX] == 11 || map.mapData[gridY][gridX] == 40) {
			playerDeath();
			Mix_PlayChannel(-1, splash, 0);
		}
		//Player restarts when touches lava
		if (map.mapData[gridY][gridX] == 13 || map.mapData[gridY][gridX] == 42) {
			playerDeath();
			Mix_PlayChannel(-1, lava, 0);
		}

		//Player picks up key on collision
		if (map.mapData[gridY][gridX] == 14) {
			pickUpKey(gridY, gridX);
		}
		//Translate the view matrix by the player's position
		viewMatrix.Identity();
		viewMatrix.Translate(-player.Position.x, -player.Position.y, 0);
	}
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
	if (keys[SDL_SCANCODE_W]) {
		int gridX, gridY;
		worldToTileCoordinates(player.Position.x, player.Position.y, &gridX, &gridY);
		FlareMap& map = chooseMap();
		if (map.mapData[gridY][gridX] == 137 || map.mapData[gridY][gridX] == 138) {
			Mix_PlayChannel(-1, doorOpen, 0);
			resetKey();
			goToNextLevel();
		}
		else if (map.mapData[gridY][gridX] == 166 || map.mapData[gridY][gridX] == 167) {
			Mix_PlayChannel(-1, doorLock, 0);
		}
	}
	if (cheat) {
		if (keys[SDL_SCANCODE_W]) {
			player.Position.y += 0.02;
		}
		if (keys[SDL_SCANCODE_A]) {
			player.Position.x -= 0.02;
			player.forward = false;
		}
		if (keys[SDL_SCANCODE_S]) {
			player.Position.y -= 0.02;
		}
		if (keys[SDL_SCANCODE_D]) {
			player.Position.x += 0.02;
			player.forward = true;
		}
	}
	else {
		if (keys[SDL_SCANCODE_A]) {
			playerIsHigh? player.acceleration.x = 1.2 : player.acceleration.x = -1.2;
			player.forward = false;
		}
		if (keys[SDL_SCANCODE_D]) {
			playerIsHigh? player.acceleration.x = -1.2 : player.acceleration.x = 1.2;
			player.forward = true;
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
				if (playerIsHigh) player.velocity.x *= -1;
				canJump = false;
			}
		}
		if (!(keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D])) {
			player.acceleration.x = 0.0;
			player.acceleration.y = 0.0;
		}
		if (!keys[SDL_SCANCODE_SPACE]) canJump = true;
	}
}

void GameState::processEvents(SDL_Event &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		float mouseX = (((float)event.button.x / 960.0f) * 7.1f) - 3.55f;
		float mouseY = (((float)(540.0f - event.button.y) / 540.0f) * 4.0f) - 2.0f;
		switch (mode) {
		case Defeat:
		case Victory:
			//Back to Menu
			if (mouseX >= -0.475f && mouseX <= 0.475f && mouseY >= -0.62f && mouseY <= -0.37f) {
				goToNextLevel();
			}
			break;
		case Menu:
			//Start Game
			if (mouseX >= -0.7f && mouseX <= 0.7f && mouseY >= -0.15f && mouseY <= 0.15f) {
				goToNextLevel();
			}
			//Instructions
			else if (mouseX >= -0.85f && mouseX <= 0.85f && mouseY >= -0.62f && mouseY <= -0.37f) {
				mode = Instruction;
			}
			//Exit Game
			else if (mouseX >= -0.625f && mouseX <= 0.625f && mouseY >= -1.15f && mouseY <= -0.85f) {
				finished = true;
			}
			break;
		case Instruction:
			//Back to Menu
			if (mouseX >= -0.85f && mouseX <= 0.85f && mouseY >= -1.75f && mouseY <= -1.45f) {
				goToNextLevel();
			}
			break;
		}
	}
	//R to restart level
	if (event.type == SDL_KEYDOWN) {
		switch (mode) {
		case Level1:
		case Level2:
		case Level3:
			if (event.key.keysym.scancode == SDL_SCANCODE_R) {
				playerDeath();
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_F1) {
				cheat = !cheat;
				player.isStatic = !player.isStatic;
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_N) {
				if (playerHasKey) {
					resetKey();
				}
				goToNextLevel();
			}
			break;
		}
	}
}

//Checks if an entity fell out of the map
bool GameState::checkEntityOutOfBounds(const Entity & other)
{
	FlareMap& map = chooseMap();
	int gridTop, gridBottom, gridLeft, gridRight;
	worldToTileCoordinates(other.Position.x - other.size.x / 2, other.Position.y - other.size.y / 2, &gridLeft, &gridBottom);
	worldToTileCoordinates(other.Position.x + other.size.x / 2, other.Position.y + other.size.y / 2, &gridRight, &gridTop);
	return (gridLeft < 0) || (gridRight >= map.mapData[0].size()) || (gridBottom >= map.mapData.size()) || (gridTop < 0);
}

//Creates entities based on the string type
void GameState::PlaceEntity(std::string type, float x, float y)
{
	if (type == "Player") {
		player.Position = Vector4(x,y,0);
		player.velocity = Vector4(0, 0, 0);
		player.acceleration = Vector4(0, 0, 0);
		player.setResetProperties();
	}
	else if (type == "Enemy") {
		Enemy enemy = Enemy(x, y, std::vector<SheetSprite>({createSheetSpriteBySpriteIndex(TextureID, 445, tileSize) }));
		enemy.acceleration.x = -0.5;
		enemy.setResetProperties();
		enemy.forward = true;
		enemies.emplace_back(enemy);
	}
	else if (type == "MovingX") {
		MovingPlatform plat = MovingPlatform(TextureID, x, y, 3);
		plat.acceleration.x = 0.3;
		plat.setResetProperties();
		platforms.emplace_back(plat);
	}
	else if (type == "MovingY") {
		MovingPlatform plat = MovingPlatform(TextureID, x, y, 3);
		plat.acceleration.y = 0.3;
		plat.setResetProperties();
		platforms.emplace_back(plat);
	}
	else if (type == "Box") {
		Entity box = Entity(x, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 191, tileSize) }), false);
		box.setResetProperties();
		boxes.emplace_back(box);
	}
	else if (type == "Ice") {
		Entity box = Entity(x, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 491, tileSize) }), false);
		box.setResetProperties();
		boxes.emplace_back(box);
	}
	else if (type == "MushroomTile") {
		mushroomTile = Entity(x, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 130, tileSize), createSheetSpriteBySpriteIndex(TextureID, 160, tileSize) }), true);
		mushroomTile.setResetProperties();
		mushroom = Entity(x, y+0.1, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 108, tileSize) }), true);
		mushroom.alpha = 0.0f;
		mushroom.setResetProperties();
	}
}

//Renders the current life count
void GameState::RenderLevelIntro(ShaderProgram& program) {
	viewMatrix.Identity();
	playerLife.Render(program, viewMatrix);
	DrawMessage(program, fontTextureID, "x ", 0.0f, 0.0f, 0.3f, -0.15f, 1.0f);
	if (playerHasDied) {
		float oldLivesAlpha = mapValue(0.75 - animationElapsed, 0, 0.75, 0.0, 1.0);
		float newLivesAlpha = mapValue(animationElapsed, 0.25, 1.0, 0.0, 1.0);
		DrawMessage(program, fontTextureID, std::to_string(player.lives + 1), 0.3f, 0.0f, 0.3f, -0.15f, oldLivesAlpha);
		DrawMessage(program, fontTextureID, std::to_string(player.lives), 0.3f, 0.0f, 0.3f, -0.15f, newLivesAlpha);
	}
	else {
		DrawMessage(program, fontTextureID, std::to_string(player.lives), 0.3f, 0.0f, 0.3f, -0.15f, 1.0f);
	}
}

//Draws the game state (tilemap and entities)
void GameState::Render(ShaderProgram & program)
{
	switch (mode) {
	case Level1:
	case Level2:
	case Level3:
		if (animationElapsed < 2) {
			RenderLevelIntro(program);
		}
		else {
			float alpha = easeInOut(0.0, 1.0, (animationElapsed - 2)*0.4);
			alpha = alpha > 1 ? 1 : alpha;
			DrawLevel(program, TextureID, chooseMap(), viewMatrix, 0.0, 0.0, alpha);
			if (alpha < 1) {
				player.alpha = alpha;
			}
			player.Render(program, viewMatrix);
			for (int i = 0; i < enemies.size(); ++i) {
				enemies[i].alpha = alpha;
				enemies[i].Render(program, viewMatrix);
			}
			for (int i = 0; i < boxes.size(); ++i) {
				boxes[i].alpha = alpha;
				boxes[i].Render(program, viewMatrix);
			}
			for (int i = 0; i < platforms.size(); ++i) {
				for (int j = 0; j < platforms[i].blocks.size(); ++j) {
					platforms[i].blocks[j].alpha = alpha;
				}
				platforms[i].Render(program, viewMatrix);
			}
			for (int i = 0; i < healthSprites.size(); ++i) {
				healthSprites[i].alpha = alpha;
				healthSprites[i].Render(program, viewMatrix);
			}
			mushroom.Render(program, viewMatrix);
			mushroomTile.alpha = alpha;
			mushroomTile.Render(program, viewMatrix);
		}
		break;
	case Victory:
		viewMatrix.Identity();
		glClearColor(0.0f, 0.659f, 0.518f, 1.0f);
		DrawMessage(program, fontTextureID, "VICTORY", -0.375f, 0.0f, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Back to Menu", -0.75f, -0.5f, 0.3f, -0.15f, 1.0f);
		break;
	case Defeat:
		viewMatrix.Identity();
		glClearColor(0.855f, 0.098f, 0.153f, 1.0f);
		DrawMessage(program, fontTextureID, "git gud", -0.375f, 0.0f, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Back to Menu", -0.75f, -0.5f, 0.3f, -0.15f, 1.0f);
		break;
	case Menu:
		viewMatrix.Identity();
		viewMatrix.Translate(-3.55f, 2.0f, 0.0f);
		glClearColor(0.553f, 0.765f, 0.855f, 0.0f);
		DrawLevel(program, TextureID, chooseMap(), viewMatrix, 0.0, 0.0, 1.0f);
		DrawMessage(program, fontTextureID, "OCTO GUAC", -1.28f, 1.0, 0.5f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Start Game", -0.6f, 0.0, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Instructions", -0.75f, -0.5, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Exit Game", -0.525f, -1.0, 0.3f, -0.15f, 1.0f);
		break;
	case Instruction:
		viewMatrix.Identity();
		viewMatrix.Translate(-3.55f, 2.0f, 0.0f);
		glClearColor(0.553f, 0.765f, 0.855f, 0.0f);
		DrawLevel(program, TextureID, chooseMap(), viewMatrix, 0.0, 0.0, 1.0f);
		DrawMessage(program, fontTextureID, "Instructions", -1.79f, 1.0f, 0.5f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "A/D : Left/Right", -1.05f, 0.2f, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "SPACE : Jump", -0.75f, -0.3f, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "R : Restart Level", -1.125f, -0.8f, 0.3f, -0.15f, 1.0f);
		DrawMessage(program, fontTextureID, "Back to Menu", -0.75f, -1.6f, 0.3f, -0.15f, 1.0f);
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
