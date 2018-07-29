#pragma once
#include "Entity.h"
class Enemy : public Entity {
public:
	Enemy(float x, float y, std::vector<SheetSprite> sprites);
	
	bool canDropDownLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const;
	bool canDropDownRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const;
	bool canJumpLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const;
	bool canJumpRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const;
};