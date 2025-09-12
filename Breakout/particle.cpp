#include "particle.h"

ParticleGen::ParticleGen(const Shader& shader, const Texture2D& texture, unsigned int amount)
	: m_particleShader{ shader }, m_particleTex{ texture }, m_PARTICLE_NR{ amount }
{
	init();
}

void ParticleGen::init()
{
	unsigned int VBO;
	constexpr std::array particleVertices{ std::to_array<float>(
		{	0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f}) };
	glGenVertexArrays(1, &m_particleVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(m_particleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);
	// prepare particles
	m_particles.reserve(m_PARTICLE_NR);
	for (std::size_t i{ 0 }; i < m_PARTICLE_NR; ++i)
	{
		m_particles.emplace_back(Particle{});
	}
}


std::size_t ParticleGen::firstVanishedParticle()
{
	// searching begins from the last record
	for (std::size_t i{ m_lastVanishedParticle }; i < m_PARTICLE_NR; ++i)
	{
		if (m_particles[i].m_life <= 0.0)
		{
			m_lastVanishedParticle = i;
			return i;
		}
	}
	// otherwise, it may be front of the last record
	for (std::size_t i{ 0 }; i < m_lastVanishedParticle; ++i)
	{
		if (m_particles[i].m_life <= 0.0)
		{
			m_lastVanishedParticle = i;
			return i;
		}
	}
	// if we find nothing eventually, we reset the record, make the first particle be overrided
	return m_lastVanishedParticle = 0;
}

void ParticleGen::respawnParticles(Particle& particle, const GameObj& obj, const glm::vec2& offset)
{
	float random{ static_cast<float>((rand() % 100) - 50) / 10.0f };
	float randColor{ 0.5f + static_cast<float>((rand() % 100) / 100.0f) };
	//float randColor2{ 0.5f + static_cast<float>((rand() % 100) / 100.0f) };
	//float randColor3{ 0.5f + static_cast<float>((rand() % 100) / 100.0f) };
	particle.m_position = obj.m_position + random + offset;
	particle.m_color = glm::vec4(randColor, randColor, randColor, 1.0f);
	particle.m_life = 1.0f;
	particle.m_velocity = obj.m_velocity * 0.1f;
}

void ParticleGen::updateParticles(float dt, int new_particleNr, const GameObj& obj, const glm::vec2& offset)
{
	// add new
	for (int i{ 0 }; i < new_particleNr; ++i)
	{
		auto firstVanishedIndex{ firstVanishedParticle() };
		respawnParticles(m_particles[firstVanishedIndex], obj, offset);
	}
	for (std::size_t i{ 0 }; i < m_PARTICLE_NR; ++i)
	{
		Particle& p{ m_particles[i] };
		p.m_life -= dt;
		if (p.m_life > 0.0)
		{
			p.m_position -= p.m_velocity * dt; // particle's dir should be the oppsite to the obj
			p.m_color.a -= dt * 2.5f; // fading
		}
	}
}

void ParticleGen::renderParticles()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // activate Particle render
	m_particleShader.use();
	for (const auto& particle : m_particles)
	{
		if (particle.m_life > 0.0f)
		{
			m_particleShader.setVec2("offset", particle.m_position);
			m_particleShader.setVec4("color", particle.m_color);
			glActiveTexture(GL_TEXTURE0);
			m_particleTex.bind();
			glBindVertexArray(m_particleVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}