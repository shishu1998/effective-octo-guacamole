#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(int TextureID, float x, float y, int numBlocks)
{
	for (int i = 0; i < numBlocks; ++i) {
		Entity block = Entity(x + tileSize * i, y, std::vector<SheetSprite>({ createSheetSpriteBySpriteIndex(TextureID, 396, tileSize) }), MovingBlock, true);
		block.setResetProperties();
		blocks.emplace_back(block);
	}
}

void MovingPlatform::Render(ShaderProgram & Program, Matrix viewMatrix, float alpha) {
	for (int i = 0; i < blocks.size(); ++i) {
		blocks[i].Render(Program, viewMatrix, alpha);
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
		if (adjustment.x != 0) {
			acceleration.x *= -1;
			velocity.x = 0;
		}
		if (adjustment.y != 0) {
			acceleration.y *= -1;
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
		if (currentBlockCollided) {
			/* Possibly include Walljump on platforms if time allows */
			//Prevents entity from falling through the platform and allowing jumping
			if (penetration.second > 0 && other.velocity.y < 0) {
				other.velocity.y = 0;
				other.collidedBottom = true;
			}
			//Checks if entity is jumping against the platform
			if (penetration.second < 0) {
				other.acceleration.y = 0;
				other.velocity.y = 0;
				other.collidedTop = true;
			}
		}
	}

	return collidesWith;
}

//Sets the reset properties of the platform
void::MovingPlatform::setResetProperties() {
	originalAcceleration = acceleration;
	originalVelocity = velocity;
}

void MovingPlatform::reset() {
	for (int i = 0; i < blocks.size(); ++i) {
		blocks[i].reset();
		acceleration = originalAcceleration;
		velocity = originalVelocity;
	}
}
