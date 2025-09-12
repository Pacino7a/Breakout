#include "gameBall.h"

Ball::Ball() // we assume ball has default radius 1.0
	: GameObj{}, m_radius{ 12.5f }, m_stucked{ true }
{}

Ball::Ball(const glm::vec2& ballPos, float radius, const glm::vec2& velocity, const Texture2D& sprite)
	: GameObj{ ballPos, glm::vec2{2 * radius}, sprite, glm::vec3{1.0f}, velocity },
	m_radius{ radius }, m_stucked{ true }, m_sticky{ false }, m_passThrough{ false }
{}

glm::vec2 Ball::move(float deltaTime, unsigned int windowWidth)
{
	if (!m_stucked)
	{
		m_position += m_velocity * deltaTime;

		if (m_position.x < 0.0) // left edge
		{
			m_velocity.x = -m_velocity.x;
			m_position.x = 0.0f;
		}
		else if ((m_position.x + (2 * m_radius)) > windowWidth) // right edge
		{
			m_velocity.x = -m_velocity.x;
			m_position.x = windowWidth - (2 * m_radius);
		}

		if (m_position.y <= 0.0f) // top edge
		{
			m_velocity.y = -m_velocity.y;
			m_position.y = 0.0f;
		}
	}

	return m_position;
}

void Ball::reset(const glm::vec2& pos, const glm::vec2& velocity)
{
	m_position = pos;
	m_velocity = velocity;
	m_stucked = true;
	m_sticky = false;
	m_passThrough = false;
}