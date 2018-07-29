#include "Player.h"

Player::Player() : Entity() {}
Player::Player(float x, float y, std::vector<SheetSprite> sprites) : Entity(x, y, sprites, false) {}