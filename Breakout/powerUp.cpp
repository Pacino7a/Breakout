#include "powerUp.h"

PowerUp::PowerUp(const std::string& type, const glm::vec3& color, float duration,
	const glm::vec2& pos, const Texture2D& texure)
	: GameObj(pos, SIZE, texure, color, VELOCITY),
	m_type{ type }, m_duration{ duration }, m_active{ false }
{}