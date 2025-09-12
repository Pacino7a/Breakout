#pragma once
#include <learnGL/shader.h>
#include <array>
#include "texture.h"


class SpriteRender
{
public:
	explicit SpriteRender() {}
	explicit SpriteRender(const Shader& spriteShader);
	~SpriteRender();

	void drawSprite(const Texture2D& spriteTex, const glm::vec2& position,
		const glm::vec2& size = glm::vec2{ 10.0f, 10.0f },
		const glm::vec3& color = glm::vec3{ 1.0f },
		float rotate = 0.0f) const;

private:
	Shader m_spriteShader{};
	unsigned int m_quadVAO{};

	void initSpriteRender();
};