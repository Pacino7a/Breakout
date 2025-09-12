#pragma once
#include "gameObj.h"

constexpr glm::vec2 SIZE{ 60.0f, 20.0f };
constexpr glm::vec2 VELOCITY{ 0.0f, 150.0f };

class PowerUp: public GameObj
{
public:
	explicit PowerUp(const std::string& type, const glm::vec3& color, float duration,
		const glm::vec2& pos, const Texture2D& texure);

	std::string m_type{};
	float m_duration{};
	bool m_active{};

};