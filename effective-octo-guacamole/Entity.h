#pragma once
#include <unordered_set>
#include "ShaderProgram.h"
#include "helper.h"
#include "SheetSprite.h"
#include "Vector4.h"
#define Friction_X 1.0f
#define Friction_Y 40.0f
#define GRAVITY -4.9f
#define AnimationConstant 0.2f

class Entity {
public:
	Entity();
	Entity(float x, float y, float width, float height, bool isStatic);
	Entity(float x, float y, std::vector<SheetSprite> sprites, bool isStatic);
	void UntexturedDraw(ShaderProgram & Program);
	void Render(ShaderProgram &Program, Matrix viewMatrix);
	void ResetContactFlags();
	bool CollidesWith(const Entity& Other);
	float TileCollideLeft(int tileX);
	float TileCollideRight(int tileX);
	float TileCollideTop(int tileY);
	float TileCollideBottom(int tileY);
	Vector4 tileCollision(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids, float displacementX, float displacementY);
	bool hasCollided() const;

	void Update(float elapsed, const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids);
	void UpdateAnimation(float elapsed);
	void Rotate(float angle);

	std::vector<std::pair<float, float>> getCorners() const;
	void remakeMatrix();
	bool SATCollidesWith(Entity& Other, std::pair<float, float>& penetration);

	void setResetProperties();
	void reset();

	std::vector<SheetSprite> sprites;
	unsigned int spriteIndex = 0;
	float animationTimer = 0;

	Vector4 Position;
	Vector4 originalPosition;
	Vector4 size;
	Vector4 velocity;
	Vector4 originalVelocity;
	Vector4 acceleration;
	Vector4 originalAcceleration;
	float alpha = 1.0;
	float Rotation = 0;

	bool isStatic;
	bool textured;
	
	Entity* parent = nullptr;

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;
	bool forward = true;

	Matrix matrix;
};