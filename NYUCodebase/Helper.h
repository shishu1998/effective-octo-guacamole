#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "stb_image.h"
#include "FlareMap.h"
#include "Entity.h"
#include <vector>
#define SPRITESHEET_WIDTH 630
#define SPRITESHEET_HEIGHT 630
#define spriteCountX 30
#define spriteCountY 30
#define tileSize 0.2f

class Entity;

GLuint LoadTexture(const char *filePath);
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing);
void DrawMessage(ShaderProgram& program, int TextureID, std::string text, float x, float y, float size, float space);
void DrawLevel(ShaderProgram& program, int textureID, FlareMap map, Matrix viewMatrix, float pos_x, float pos_y);
SheetSprite& createSheetSprite(unsigned int textureID, float x, float y, float width, float height, float size);
SheetSprite& createSheetSpriteBySpriteIndex(unsigned int textureID, int spriteIndex, float size);
float lerp(float v0, float v1, float t);
void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);