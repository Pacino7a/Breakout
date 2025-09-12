#define STB_IMAGE_IMPLEMENTATION
#include "resourceManager.h"

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture2D> ResourceManager::textures;

Shader ResourceManager::loadShader(const std::string& name, const char* vShaderfile, const char* fShaderfile, const char* gShaderfile)
{
	return shaders[name] = loadShadersFromFile(vShaderfile, fShaderfile, gShaderfile);
}

Shader ResourceManager::getShader(const std::string& name)
{
	return shaders[name];
}

Texture2D ResourceManager::loadTexture(const char* textureFile, bool alpha, const std::string& name)
{
	return textures[name] = loadTextureFromFile(textureFile, alpha);
}

Texture2D ResourceManager::getTexture(const std::string& name)
{
	return textures[name];
}

void ResourceManager::clearResource()
{
	for (auto& [name, shader] : shaders)
		glDeleteProgram(shader.ID);
	for (auto& [name, texture] : textures)
		glDeleteTextures(1, &texture.m_ID);
}

Shader ResourceManager::loadShadersFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		std::ifstream vertexShaderFile{ vShaderFile }; // open file
		std::ifstream fragmentShaderFile{ fShaderFile };
		std::stringstream vertexStream, fragmentStream; // set Stream
		vertexStream << vertexShaderFile.rdbuf(); // read file to Stream
		fragmentStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close(); // close file
		fragmentShaderFile.close();
		vertexCode = vertexStream.str(); // stream -> string
		fragmentCode = fragmentStream.str();
		if (gShaderFile)
		{
			std::ifstream geometryShaderFile{ gShaderFile };
			std::stringstream geometryStream;
			geometryStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = geometryStream.str();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char* vShaderCode{ vertexCode.c_str() };
	const char* fShaderCode{ fragmentCode.c_str() };
	const char* gShaderCode{ geometryCode.c_str() };
	Shader shader{};
	shader.compile(vShaderCode, gShaderFile ? gShaderCode : nullptr, fShaderCode);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* textureFile, bool alpha)
{
	Texture2D texture{};
	if (alpha)
	{
		texture.m_internalFormat = GL_RGBA;
		texture.m_loadFormat = GL_RGBA;
	}

	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data{ stbi_load(textureFile, &width, &height, &nrChannels, 0) };
	texture.generate(width, height, data);
	stbi_image_free(data);
	return texture;
}