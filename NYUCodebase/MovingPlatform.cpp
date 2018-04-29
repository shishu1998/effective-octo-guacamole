#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(int TextureID, float x, float y, int numBlocks)
{
	for (int i = 0; i < numBlocks; ++i) {
		blocks.emplace_back(Entity(x + tileSize*i, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 124, tileSize) }), MovingBlock, true));
	}
}

void MovingPlatform::Render(ShaderProgram & Program, Matrix viewMatrix) {
	for (int i = 0; i < blocks.size(); ++i) {
		blocks[i].Render(Program, viewMatrix);
	}
}

void MovingPlatform::Update(float elapsed, const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids, Entity& entity) {
	velocity.x = lerp(velocity.x, 0.0f, elapsed * Friction_X);
	velocity.x += acceleration.x * elapsed;
	velocity.y += acceleration.y * elapsed;

	float displacementX = velocity.x * elapsed;
	float displacementY = velocity.y * elapsed;

	for (int i = 0; i < blocks.size(); ++i) {
		Vector4 adjustment = blocks[i].tileCollision(mapData, solids, displacementX, displacementY);
		for (int j = 0; j < blocks.size(); ++j) {
			if (i != j) {
				blocks[j].Position.x += adjustment.x;
				blocks[j].Position.y += adjustment.y;
			}
		}
		entity.Position.x += adjustment.x;
		entity.Position.y += adjustment.y;
		if (adjustment.x > 0) {
			acceleration.x = 0.3;
			velocity.x = 0;
		}
		else if (adjustment.x < 0) {
			acceleration.x = -0.3;
			velocity.x = 0;
		}
		if (adjustment.y > 0) {
			acceleration.y = 0.3;
			velocity.y = 0;
		}
		else if (adjustment.y < 0) {
			acceleration.y = -0.3;
			velocity.y = 0;
		}
	}
	if (CollidesWith(entity)) {
		entity.Position.x += displacementX;
		entity.Position.y += displacementY;
	}
}

bool MovingPlatform::CollidesWith(Entity & other)
{
	bool collidesWith = false;
	std::pair<float, float> penetration;
	for (int i = 0; i < blocks.size(); ++i) {
		bool currentBlockCollided = other.SATCollidesWith(blocks[i], penetration);
		collidesWith = collidesWith || currentBlockCollided;
		//Prevents entity from falling through the platform and allowing jumping
		if (currentBlockCollided && penetration.second > 0 && other.velocity.y < 0) {
			other.velocity.y = 0;
			other.collidedBottom = true;
			//If one tile collides and they're all aligned horizontally, there's no point to continue checking
			break;
		}
	}

	return collidesWith;
}
