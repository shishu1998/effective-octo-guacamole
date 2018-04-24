#include "Helper.h"
#include <assert.h>

// Loads Textures and return the Texture ID
GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(image);
	return retTexture;
}

// Text drawing
void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
			});
	}
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, (int)text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

// Draws messages on a given x,y coordinate
void DrawMessage(ShaderProgram& program, int TextureID, std::string text, float x, float y, float size, float space) {
	Matrix modelMatrix;
	Matrix viewMatrix;
	modelMatrix.Translate(x,y,0);
	program.SetModelMatrix(modelMatrix);
	program.SetViewMatrix(viewMatrix);
	DrawText(&program, TextureID, text, size, space);
}

//Draw the level starting from an x,y coordinate using a FlareMap, takes in a viewMatrix for a view
void DrawLevel(ShaderProgram & program, int textureID, FlareMap map, Matrix viewMatrix, float pos_x, float pos_y)
{
	Matrix modelMatrix;
	modelMatrix.Translate(pos_x, pos_y, 0);

	program.SetModelMatrix(modelMatrix);
	program.SetViewMatrix(viewMatrix);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int y = 0; y < map.mapHeight; y++) {
		for (int x = 0; x < map.mapWidth; x++) {
			if (map.mapData[y][x] != 0) {
				float u = (float)(map.mapData[y][x] % spriteCountX) / (float)spriteCountX;
				float v = (float)(map.mapData[y][x] / spriteCountX) / (float)spriteCountY;
				float spriteWidth = 1.0f / (float)spriteCountX;
				float spriteHeight = 1.0f / (float)spriteCountY;
				vertexData.insert(vertexData.end(), {
					tileSize * x, -tileSize * y,
					tileSize * x, (-tileSize * y) - tileSize,
					(tileSize * x) + tileSize, (-tileSize * y) - tileSize,

					tileSize * x, -tileSize * y,
					(tileSize * x) + tileSize, (-tileSize * y) - tileSize,
					(tileSize * x) + tileSize, -tileSize * y
					});
				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
					});
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, (int)vertexData.size() / 2);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

// Normalize the coordinates of the sprite sheet and create a sheetsprite
SheetSprite& createSheetSprite(unsigned int textureID, float x, float y, float width, float height, float size) {
	return SheetSprite(textureID, x / SPRITESHEET_WIDTH, y / SPRITESHEET_HEIGHT, width / SPRITESHEET_WIDTH, height / SPRITESHEET_HEIGHT, size);
}

SheetSprite& createSheetSpriteBySpriteIndex(unsigned int textureID, int spriteIndex, float size)
{
	{
		int spriteSheetx = spriteIndex % spriteCountX;
		int spriteSheetY = spriteIndex / spriteCountX;
		SheetSprite sprite = createSheetSprite(textureID, spriteSheetx * 21.0f, spriteSheetY * 21.0f, 21.0f, 21.0f, size);
		return sprite;
	}
}

float lerp(float v0, float v1, float t)
{
	return (1.0 - t)*v0 + t * v1;
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / tileSize);
	*gridY = (int)(-worldY / tileSize);
}
