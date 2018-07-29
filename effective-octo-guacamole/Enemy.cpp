#include "Enemy.h"

Enemy::Enemy(float x, float y, std::vector<SheetSprite> sprites) : Entity(x, y, sprites, false) {}

// Checks if the entity can drop down left to another solid tile below
bool Enemy::canDropDownLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);
	for (int i = 1; i < 5; ++i) {
		if ((solids.find(mapData[gridY + i][gridX - 1]) != solids.end())) {
			return true;
		}
	}

	return false;
}

// Checks if the entity can drop down right to another solid tile below
bool Enemy::canDropDownRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);
	for (int i = 1; i < 5; ++i) {
		if ((solids.find(mapData[gridY + i][gridX + 1]) != solids.end())) {
			return true;
		}
	}

	return false;
}

// Checks if the entity can jump to a tile on the left
bool Enemy::canJumpLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);

	float canJumpLeft = false;
	// Checks if we're going to the left
	if (velocity.x < 0) {
		//If there's an obstacle in front, you can jump over it if it's shorter than 2 tiles high
		canJumpLeft = (solids.find(mapData[gridY][gridX - 2]) != solids.end() || solids.find(mapData[gridY - 1][gridX - 2]) != solids.end()) && solids.find(mapData[gridY - 2][gridX - 2]) == solids.end();
	}
	return canJumpLeft;
}

// Checks if the entity can jump to a tile on the right
bool Enemy::canJumpRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);

	float canJumpRight = false;
	// Checks if we're going to the right
	if (velocity.x > 0) {
		//If there's an obstacle in front, you can jump over it if it's shorter than 2 tiles high
		canJumpRight = (solids.find(mapData[gridY][gridX + 2]) != solids.end() || solids.find(mapData[gridY - 1][gridX + 2]) != solids.end()) && solids.find(mapData[gridY - 2][gridX + 2]) == solids.end();
	}
	return canJumpRight;
}