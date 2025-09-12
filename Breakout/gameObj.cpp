#include "gameObj.h"

GameObj::GameObj()
	: m_position{ 0.0f }, m_size{ 1.0f, 1.0f }, m_velocity{ 0.0f }, m_sprite{},
	m_color{ 1.0f }, m_rotation{ 0.0f }, m_isSolid{ false }, m_destroyed{ false }
{}

GameObj::GameObj(const glm::vec2& pos, const glm::vec2& size, const Texture2D& sprite,
	const glm::vec3& color, const glm::vec2& velocity)
	: m_position{ pos }, m_size{ size }, m_velocity{ velocity }, m_sprite{ sprite },
	m_color{ color }, m_rotation{ 0.0f }, m_isSolid{ false }, m_destroyed{ false }
{}

bool GameObj::isSolid() const
{
	return m_isSolid;
}

bool GameObj::isDestroyed() const
{
	return m_destroyed;
}

void GameObj::draw(const SpriteRender& spriteRender) const
{
	spriteRender.drawSprite(m_sprite, m_position, m_size, m_color, m_rotation); // because we have used Virtual,
																 // so derived class' draw() will override this.
}