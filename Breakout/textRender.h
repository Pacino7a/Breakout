#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <learnGL/shader.h>
#include <map>
#include <array>
#include "resourceManager.h"


/* for trueType */
struct Character
{
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class TextRender
{
public:
	explicit TextRender(unsigned int width, unsigned int height);

	std::map<char, Character> m_characters{};
	
	Shader m_textShader{};

	void load(const std::string& fontPath, unsigned int fontSize);
	void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color = glm::vec3{ 1.0f });

private:
	unsigned int m_VAO{};
	unsigned int m_VBO{};
};