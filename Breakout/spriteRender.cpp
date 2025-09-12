#include "spriteRender.h"

SpriteRender::SpriteRender(const Shader& spriteShader)
	: m_spriteShader{ spriteShader }
{
	//m_spriteShader.use();
	//m_spriteShader.setInt("spriteTexture", 0);
	initSpriteRender();
}

SpriteRender::~SpriteRender()
{
	glDeleteVertexArrays(1, &m_quadVAO);
}

void SpriteRender::drawSprite(const Texture2D& spriteTex, const glm::vec2& position, const glm::vec2& size,
	const glm::vec3& color, float rotate) const
{
	m_spriteShader.use();
	glm::mat4 model{ 1.0f };

	//std::cout << "Pos " << position.x << ", " << position.y << '\n';
	//std::cout << "Scale " << size.x << ", " << size.y << '\n';

	model = glm::translate(model, glm::vec3{ position, 0.0f });

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), glm::vec3{ 0.0f, 0.0f, 1.0f });
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3{ size, 1.0f });

	m_spriteShader.setMat4("model", model);
	m_spriteShader.setVec3("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	spriteTex.bind();

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void SpriteRender::initSpriteRender()
{
	unsigned int VBO{};
	constexpr std::array<float, 24> vertices{ // local Coords  should be aligned with Texcoords
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}