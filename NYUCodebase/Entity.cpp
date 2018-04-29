#include "Entity.h"
#include "Helper.h"
#include "SatCollision.h"

Entity::Entity() : textured(false) {}
Entity::Entity(float x, float y, float width, float height, bool isStatic) : Position(x, y, 0), size(width, height, 0), isStatic(isStatic),textured(false) {
	matrix.Translate(Position.x, Position.y, 0);
}
Entity::Entity(float x, float y, std::vector<SheetSprite> sprites, EntityType type, bool isStatic) : Position(x,y,0), 
size(sprites[0].width * sprites[0].size/ sprites[0].height, sprites[0].size, 0), sprites(sprites), entityType(type), isStatic(isStatic), textured(true) {
	matrix.Translate(Position.x, Position.y, 0);
}

void Entity::UntexturedDraw(ShaderProgram & Program) {
	glUseProgram(Program.programID);
	float vertices[] = {
		-0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
		0.5f, 0.5f,
		-0.5f, -0.5f,
		0.5f, -0.5f };

	glVertexAttribPointer(Program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(Program.positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(Program.positionAttribute);
}

void Entity::Render(ShaderProgram & Program, Matrix viewMatrix)
{
	remakeMatrix();
	Matrix modelMatrix = matrix;
	if (!forward) modelMatrix.Scale(-1.0, 1.0, 0);

	Program.SetModelMatrix(modelMatrix);
	Program.SetViewMatrix(viewMatrix);
	 
	textured ? sprites[spriteIndex].Draw(&Program) : UntexturedDraw(Program);
}

//Resets contact flags
void Entity::ResetContactFlags() {
	collidedLeft = collidedRight = collidedTop = collidedBottom = false;
}

//Checks collision with another entity
bool Entity::CollidesWith(const Entity& Other)
{
	return !(Position.y - size.y / 2 > Other.Position.y + Other.size.y / 2 || Position.y + size.y / 2 < Other.Position.y - Other.size.y / 2 || Position.x - size.x / 2 > Other.Position.x + Other.size.x / 2 || Position.x + size.x / 2 < Other.Position.x - Other.size.x / 2);
}

//Left Tile Collision adjustment, returns the adjustment amount
float Entity::TileCollideLeft(int tileX) {
	float worldX = tileX * tileSize;
	collidedLeft = true;
	acceleration.x = 0;
	velocity.x = 0;
	float leftPen = fabs((worldX + tileSize) - (Position.x - size.x / 2)) + 0.0001*tileSize;
	Position.x += leftPen;
	return leftPen;
}

//Right Tile Collision adjustment, returns the adjustment amount
float Entity::TileCollideRight(int tileX) {
	float worldX = tileX * tileSize;
	collidedRight = true;
	acceleration.x = 0;
	velocity.x = 0;
	float rightPen = fabs(Position.x + size.x / 2 - worldX) + 0.0001*tileSize;
	Position.x -= rightPen;
	return -rightPen;
}

//Top Tile Collision adjustment, returns the adjustment amount
float Entity::TileCollideTop(int tileY) {
	float worldY = tileY * -tileSize;
	collidedTop = true;
	acceleration.y = 0;
	velocity.y = 0;
	float topPen = fabs((Position.y + size.y / 2) - (worldY - tileSize)) + tileSize * 0.0001;
	Position.y -= topPen;
	return -topPen;
}

//Bottom Tile Collision adjustment, returns the adjustment amount
float Entity::TileCollideBottom(int tileY) {
	float worldY = tileY * -tileSize;
	collidedBottom = true;
	acceleration.y = 0;
	velocity.y = 0;
	float botPen = fabs(worldY - (Position.y - (size.y / 2))) + tileSize * 0.0001;
	Position.y += botPen;
	return botPen;
}

//Resolves tile collisions on all sides, returns a vector of all the adjustments made to resolve the collisions
Vector4 Entity::tileCollision(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids, float displacementX, float displacementY)
{
	int gridX, gridY, gridLeft, gridRight, gridTop, gridBottom;
	int gridQuarterLeft, gridQuarterRight, gridQuarterTop, gridQuarterBottom;

	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);
	worldToTileCoordinates(Position.x - size.x / 2, Position.y - size.y / 2, &gridLeft, &gridBottom);
	worldToTileCoordinates(Position.x + size.x / 2, Position.y + size.y / 2, &gridRight, &gridTop);
	worldToTileCoordinates(Position.x - size.x / 4, Position.y - size.y / 4, &gridQuarterLeft, &gridQuarterBottom);
	worldToTileCoordinates(Position.x + size.x / 4, Position.y + size.x / 4, &gridQuarterRight, &gridQuarterTop);

	Vector4 adjustment;

	// X TileCollision 
	Position.x += displacementX;
	//Left side: Center, Top Quarter, Bottom Quarter
	if ((solids.find(mapData[gridY][gridLeft]) != solids.end()) ||
		(solids.find(mapData[gridQuarterTop][gridLeft]) != solids.end()) ||
		(solids.find(mapData[gridQuarterBottom][gridLeft]) != solids.end())) {
		adjustment.x += TileCollideLeft(gridLeft);
	}
	//Right side: Center, Top Quarter, Bottom Quarter
	if ((solids.find(mapData[gridY][gridRight]) != solids.end()) ||
		(solids.find(mapData[gridQuarterTop][gridRight]) != solids.end()) ||
		(solids.find(mapData[gridQuarterBottom][gridRight]) != solids.end())) {
		adjustment.x += TileCollideRight(gridRight);
	}

	// Y TileCollision
	Position.y += displacementY;
	//Top Side: Center, Left Quarter, Right Quarter
	if ((solids.find(mapData[gridTop][gridX]) != solids.end()) ||
		(solids.find(mapData[gridTop][gridQuarterLeft]) != solids.end()) ||
		(solids.find(mapData[gridTop][gridQuarterRight]) != solids.end())) {
		adjustment.y += TileCollideTop(gridTop);
	}
	//Bottom Side: Center, Left Quarter, Right Quarter
	if ((solids.find(mapData[gridBottom][gridX]) != solids.end()) ||
		(solids.find(mapData[gridBottom][gridQuarterLeft]) != solids.end()) ||
		(solids.find(mapData[gridBottom][gridQuarterRight]) != solids.end())) {
		adjustment.y += TileCollideBottom(gridBottom);
	}

	return adjustment;
}

// Returns if Collision has occured
bool Entity::hasCollided() const{
	return collidedTop || collidedBottom || collidedLeft || collidedRight;
}

void Entity::remakeMatrix() {
	matrix.Identity();
	matrix.Translate(Position.x, Position.y, Position.z);
	matrix.Rotate(Rotation);
	matrix.Scale(size.x, size.y, size.z);
}

//Updates the position of current entity and adjusts for collision
void Entity::Update(float elapsed, const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids)
{
	ResetContactFlags();
	if (!isStatic) {
	
		velocity.x = lerp(velocity.x, 0.0f, elapsed * Friction_X);
		velocity.x += acceleration.x * elapsed;
		velocity.y += GRAVITY * elapsed;

		float displacementX = velocity.x * elapsed;
		float displacementY = velocity.y * elapsed;

		tileCollision(mapData, solids, displacementX, displacementY);

		// Apply kinetic friction if dropping down a wall
		if(collidedLeft || collidedRight)
			velocity.y = lerp(velocity.y, 0.0f, elapsed * Friction_Y);
		UpdateAnimation(elapsed);
	}
}

// Updates the sprite index of the entity
void Entity::UpdateAnimation(float elapsed) {
	// Should only update when sprite is moving(slow velocity doesn't count)
	if (fabs(velocity.x) > 0.05) {
		animationTimer += elapsed;
		//Animation update time should be inversely proportional to the velocity
		if (animationTimer > AnimationConstant / fabs(velocity.x)) {
			spriteIndex = (spriteIndex + 1) % sprites.size();
			animationTimer = 0;
		}
	}
	else {
		// Reset sprite index if too slow/not moving
		spriteIndex = 0;
	}
}

void Entity::Rotate(float angle) {
	Rotation += angle;
}

//Gets the corners of the entity
std::vector<std::pair<float, float>> Entity::getCorners() const {
	std::vector<std::pair<float, float>> vertices = std::vector<std::pair<float, float>>();
	Vector4 topLeft = matrix * Vector4(-0.5, 0.5, 0);
	Vector4 topRight = matrix * Vector4(0.5, 0.5, 0);
	Vector4 botLeft = matrix * Vector4(-0.5, -0.5, 0);
	Vector4 botRight = matrix * Vector4(0.5, -0.5, 0);
	vertices.emplace_back(topLeft.x, topLeft.y);
	vertices.emplace_back(topRight.x, topRight.y);
	vertices.emplace_back(botRight.x, botRight.y);
	vertices.emplace_back(botLeft.x, botLeft.y);
	return vertices;
}

//SAT Collision code
bool Entity::SATCollidesWith(Entity& Other) {
	std::pair<float, float> penetration;
	bool collided = CheckSATCollision(getCorners(), Other.getCorners(), penetration);
	if (Other.isStatic) {
		Position.x += penetration.first;
		Position.y += penetration.second;
	}
	else {
		Position.x += (penetration.first * 0.5f);
		Position.y += (penetration.second * 0.5f);
		Other.Position.x -= (penetration.first * 0.5f);
		Other.Position.y -= (penetration.second * 0.5f);
	}
	return collided;
}

// Checks if the entity can drop down left to another solid tile below
bool Entity::canDropDownLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);
	for (int i = 1; i < 5; ++i) {
		if ((solids.find(mapData[gridY+i][gridX-1]) != solids.end())) {
			return true;
		}
	}

	return false;
}

// Checks if the entity can drop down right to another solid tile below
bool Entity::canDropDownRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
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
bool Entity::canJumpLeft(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);

	// Checks if we're going to the left
	if (velocity.x < 0) {
		for (int i = 1; i < 3; ++i) {
			if ((solids.find(mapData[gridY - i][gridX - 2]) != solids.end())) {
				return true;
			}
		}
	}

	return false;
}

// Checks if the entity can jump to a tile on the right
bool Entity::canJumpRight(const std::vector<std::vector<unsigned int>>& mapData, std::unordered_set<int>& solids) const
{
	int gridX, gridY;
	worldToTileCoordinates(Position.x, Position.y, &gridX, &gridY);

	// Checks if we're going to the right
	if (velocity.x > 0) {
		for (int i = 1; i < 3; ++i) {
			if ((solids.find(mapData[gridY - i][gridX + 2]) != solids.end())) {
				return true;
			}
		}
	}

	return false;
}
