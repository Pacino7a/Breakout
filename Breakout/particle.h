#pragma once
#include <learnGL/shader.h>
#include <vector>
#include <array>
#include "texture.h"
#include "gameObj.h"

struct Particle
{
	glm::vec2 m_position, m_velocity;
	glm::vec4 m_color;
	float m_life;

	explicit Particle()
		: m_position{ 0.0f }, m_velocity{ 0.0f }, m_color{ 1.0f }, m_life{ 0.0f }
	{}
};


class ParticleGen
{
public:

	explicit ParticleGen(const Shader& shader, const Texture2D& texture, unsigned int amount);
	//~ParticleGen();

	void updateParticles(float dt, int new_particleNr, const GameObj& obj, const glm::vec2& offset = glm::vec2{0.0f});
	void renderParticles();

private:
	
	std::size_t m_PARTICLE_NR{};
	std::size_t m_lastVanishedParticle{};
	
	unsigned int m_particleVAO{};
	std::vector<Particle> m_particles{};

	Shader m_particleShader{};
	Texture2D m_particleTex{};
	void init();
	std::size_t firstVanishedParticle();
	void respawnParticles(Particle& particle, const GameObj& obj, const glm::vec2& offset = glm::vec2{0.0f});
};