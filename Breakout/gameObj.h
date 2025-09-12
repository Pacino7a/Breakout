#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h"
#include "spriteRender.h"


class GameObj // base Class
{
public:
	explicit GameObj();
	explicit GameObj(const glm::vec2& pos, const glm::vec2& size, const Texture2D& sprite,
					 const glm::vec3& color = glm::vec3(1.0f), const glm::vec2& velocity = glm::vec2(0.0f));

	glm::vec2 m_position{};
	glm::vec2 m_size{};
	glm::vec2 m_velocity{}; // for Ball
	glm::vec3 m_color{};

	Texture2D m_sprite;

	float m_rotation{}; // for ball
	bool m_isSolid{}; // for brick
	bool m_destroyed{}; // for brick

	bool isSolid() const;
	bool isDestroyed() const;
	virtual void draw(const SpriteRender& spriteRender) const;

};

