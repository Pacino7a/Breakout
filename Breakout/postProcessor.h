#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>
#include "texture.h"
#include "spriteRender.h"
#include <learnGL/shader.h>

class PostProcessor
{
public:
	explicit PostProcessor(const Shader& shader, unsigned int width, unsigned int height);

	unsigned int m_width{};
	unsigned int m_height{};
	bool m_chaos{};
	bool m_confuse{};
	bool m_shake{};
	Texture2D m_scene{};
	Shader m_scrShader{};

	void preparation();
	void render2FBO();
	void render2SCR(float time);

private:
	unsigned int m_MSFBO{}; // for multisampling -- Anti-Aliasings
	unsigned int m_FBO{};
	unsigned int m_RBO{};
	unsigned int m_VAO{};

	void init();
};
