#include "texture.h"

Texture2D::Texture2D()
	: m_width{ 0 }, m_height{ 0 },
	m_internalFormat{ GL_RGB }, m_loadFormat{ GL_RGB },
	m_warpS{ GL_REPEAT }, m_warpT{ GL_REPEAT },
	m_minFilter{ GL_LINEAR }, m_magFilter{ GL_LINEAR }
{
	glGenTextures(1, &m_ID);
}

void Texture2D::generate(unsigned int width, unsigned int height, unsigned char* data)
{
	m_width = width;
	m_height = height;

	glBindTexture(GL_TEXTURE_2D, m_ID);
	// transpass data into obj
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_loadFormat, GL_UNSIGNED_BYTE, data);
	// set configurations
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_warpS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_warpT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}