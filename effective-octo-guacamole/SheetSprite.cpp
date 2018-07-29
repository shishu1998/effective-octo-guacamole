#include "SheetSprite.h"

SheetSprite::SheetSprite() {}
SheetSprite::SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size): textureID(textureID), u(u), v(v), width(width), height(height), size(size){}

void SheetSprite::Draw(ShaderProgram * program)
{
	glUseProgram(program->programID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
		-0.5f * aspect, -0.5f,
		0.5f * aspect, 0.5f,
		-0.5f * aspect, 0.5f,
		0.5f * aspect, 0.5f,
		-0.5f * aspect, -0.5f,
		0.5f * aspect, -0.5f};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}
