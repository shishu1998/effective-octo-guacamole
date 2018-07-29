#pragma once
#include "Entity.h"
class Player : public Entity {
public:
	int health;
	int lives;

	Player();
	Player(float x, float y, std::vector<SheetSprite> sprites);
};