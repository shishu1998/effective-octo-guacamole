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
	for (int i = 0; i < blocks.size(); ++i) {
		collidesWith = collidesWith || other.SATCollidesWith(blocks[i]);
	}

	//If the entity is on top of the platform, set y velocity to 0
	if (collidesWith) {
		float entityBot = other.Position.y - other.size.y / 2;
		//Tile Collision checks quarter segments, we're pretending the platforms are like tiles
		float entityQuarterRight = other.Position.x + tileSize/4;
		float entityQuarterLeft = other.Position.x - tileSize / 4;
		float firstBlockLeft = blocks.front().Position.x - blocks.front().size.x / 2;
		float lastBlockRight = blocks.back().Position.x + blocks.back().size.x / 2;
		if (entityBot > blocks[0].Position.y && (entityQuarterRight > firstBlockLeft || entityQuarterLeft < lastBlockRight)) {
			other.collidedBottom = true;
			//Prevents the entity from dropping down the platform
			if (other.velocity.y < 0) {
				other.velocity.y = 0;
			}
		}
	}
	return collidesWith;
}
