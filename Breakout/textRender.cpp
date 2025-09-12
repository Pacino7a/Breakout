#include "textRender.h"

TextRender::TextRender(unsigned int width, unsigned int height)
{
	m_textShader = ResourceManager::loadShader("text", "./shaders/Font.vert", "./shaders/Font.frag");
	m_textShader.use();
	m_textShader.setMat4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f));
	m_textShader.setInt("fontBitMap", 0);
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRender::load(const std::string& fontPath, unsigned int fontSize) // load font lib and create glyphs for bitmap
{
	m_characters.clear();

	FT_Library ft; // init
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face; // load library
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, fontSize); // set glyph size
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c{ 0 }; c < 128; ++c) // load 128 ascii glyphs and generate their bitmap
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		unsigned int texture{};
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
			face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character charcter{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x };
		m_characters.insert(std::pair<char, Character>{ c, charcter });
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRender::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
{
	m_textShader.use();
	m_textShader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_VAO);

	for (std::string::const_iterator ch{ text.begin() }; ch != text.end(); ++ch)
	{
		Character character{ m_characters[*ch] }; // extract specific character from the LUT we made

		// start
		float xpos{ x + character.Bearing.x * scale };
		float ypos{ y + (m_characters['H'].Bearing.y - character.Bearing.y) * scale }; // if you use size here, the offset will be wrong
		// size
		float w{ character.Size.x * scale };
		float h{ character.Size.y * scale };

		std::array<std::array<float, 4>, 6> vertices{ {
			{ xpos,     ypos + h,   0.0f, 1.0f }, // because we use Vertical Inversed Ortho Projection, 
			{ xpos + w, ypos,       1.0f, 0.0f }, // so the uv coordinates are on longer need to flip on Y
			{ xpos,     ypos,       0.0f, 0.0f }, // the Origin is on the left top corner , so rendering Order is CCW

			{ xpos,     ypos + h,   0.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 0.0f }
		} };
		glBindTexture(GL_TEXTURE_2D, character.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data()); // we have already allocate the Buffer, so we use SubData
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (character.Advance >> 6) * scale; //  1/64 pixel
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}