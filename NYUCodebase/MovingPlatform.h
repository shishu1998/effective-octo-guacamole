#pragma once
#include "Entity.h"
#include <vector>

class MovingPlatform {
public:
	std::vector<Entity> blocks;
	Vector4 velocity;
	Vector4 acceleration;
	MovingPlatform(int TextureID, float x, float y, int numBlocks);
	void MovingPlatform::Render(ShaderProgram & Program, Matrix viewMatrix);
	void MovingPlatform::Update(float elapsed, const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids, Entity& player);
	bool MovingPlatform::CollidesWith(Entity& other);
};