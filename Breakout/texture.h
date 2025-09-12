#pragma once
#include <glad/glad.h>

class Texture2D
{
public:
	explicit Texture2D(); // init
	void generate(unsigned int width, unsigned int height, unsigned char* data);
	void bind() const;

	unsigned int m_ID{};
	unsigned int m_width{};
	unsigned int m_height{};
	unsigned int m_internalFormat{};
	unsigned int m_loadFormat{};
	unsigned int m_warpS{};
	unsigned int m_warpT{};
	unsigned int m_minFilter{};
	unsigned int m_magFilter{};
};