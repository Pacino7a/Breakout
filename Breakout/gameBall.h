#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObj.h"

class Ball : public GameObj
{
public:
	explicit Ball();
	explicit Ball(const glm::vec2& ballPos, float radius, const glm::vec2& velocity, const Texture2D& sprite);
	float m_radius{};
	bool m_stucked{};
	bool m_sticky{};
	bool m_passThrough{};

	glm::vec2 move(float deltaTime, unsigned int windowWidth);
	void reset(const glm::vec2& pos, const glm::vec2& velocity);
};