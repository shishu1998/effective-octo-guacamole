#pragma once
#include <unordered_set>
#include "ShaderProgram.h"
#include "helper.h"
#include "SheetSprite.h"
#include "Vector4.h"
#define Friction_X 1.0f
#define GRAVITY -0.8f
#define AnimationConstant 0.2f

enum EntityType {Player, Enemy};
class Entity {
public:
	Entity();
	Entity(float x, float y, float width, float height, bool isStatic);
	Entity(float x, float y, std::vector<SheetSprite> sprites, EntityType type, bool isStatic);
	void UntexturedDraw(ShaderProgram & Program);
	void Render(ShaderProgram &Program, Matrix viewMatrix);
	void ResetContactFlags();
	bool CollidesWith(const Entity& Other);
	void TileCollideLeft(int tileX);
	void TileCollideRight(int tileX);
	void TileCollideTop(int tileY);
	void TileCollideBottom(int tileY);
	bool hasCollided() const;

	void Update(float elapsed, const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int> solids);
	void Rotate(float angle);

	std::vector<std::pair<float, float>> getCorners() const;
	void remakeMatrix();
	bool SATCollidesWith(Entity& Other);

	std::vector<SheetSprite> sprites;
	unsigned int spriteIndex = 0;
	float animationTimer = 0;

	Vector4 Position;
	Vector4 size;
	Vector4 velocity;
	Vector4 acceleration;
	float Rotation = 0;

	bool isStatic;
	bool textured;
	EntityType entityType;
	
	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;
	bool forward = true;

	Matrix matrix;
};