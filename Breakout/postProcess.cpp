#include "postProcessor.h"

PostProcessor::PostProcessor(const Shader& shader, unsigned int width, unsigned int height)
	: m_scrShader{ shader }, m_width{ width }, m_height{ height },
	  m_confuse{false}, m_chaos{false}, m_shake{false}
{
	init();
}

void PostProcessor::preparation()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // our scene is a 2D scene, so Depth clear is no need
}

void PostProcessor::render2FBO()
{
	// resolve the multisampled color buffer into normal FBO, to store to Texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO); // bind FBO, blit render result to FBO's Color attachment
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // eventually, prepare for rendering to SCR
}

void PostProcessor::render2SCR(float time)
{
	m_scrShader.use();
	m_scrShader.setFloat("time", time);
	m_scrShader.setBool("confuse", m_confuse);
	m_scrShader.setBool("chaos", m_chaos);
	m_scrShader.setBool("shake", m_shake);

	glActiveTexture(GL_TEXTURE0);
	m_scene.bind();
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}

void PostProcessor::init()
{
	// set multisample FBO and PostProcess FBO
	glGenFramebuffers(1, &m_MSFBO);
	glGenFramebuffers(1, &m_FBO);
	glGenRenderbuffers(1, &m_RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); // we need to do some postprocess to the rendering result, so we create a Texture for sampling
	m_scene.generate(m_width, m_height, NULL); // bind m_scene as Attachment of FBO for sampling
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_scene.m_ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// initial data
	unsigned int VBO{};
	constexpr std::array vertices{ std::to_array<float>({
		// pos        // tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
		}) };

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// set shader
	m_scrShader.use();
	m_scrShader.setInt("renderResult", 0);
	constexpr float POST_OFFSET{ 1.0f / 300.0f };
	constexpr std::array<std::array<float, 2>, 9> POST_OFFSET_ARR{ {
		{-POST_OFFSET,  POST_OFFSET},
		{		 0.0f,  POST_OFFSET},
		{ POST_OFFSET,  POST_OFFSET},
		{-POST_OFFSET,		   0.0f},
		{		 0.0f,		   0.0f},
		{ POST_OFFSET,		   0.0f},
		{-POST_OFFSET, -POST_OFFSET},
		{		 0.0f, -POST_OFFSET},
		{ POST_OFFSET, -POST_OFFSET}
	}};
	glUniform2fv(glGetUniformLocation(this->m_scrShader.ID, "offsets"), 9, (float*)POST_OFFSET_ARR.data());

	constexpr std::array edge_kernel{
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->m_scrShader.ID, "edge_kernel"), 9, edge_kernel.data());

	constexpr std::array blur_kernel{
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
	};
	glUniform1fv(glGetUniformLocation(this->m_scrShader.ID, "blur_kernel"), 9, blur_kernel.data());
}