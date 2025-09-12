#pragma once
#include <map>
#include <string>
#include <glad/glad.h>
#include <learnGL/shader.h>
#include <learnGL/stb_image.h>
#include "texture.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture2D> textures;

	static Shader loadShader(const std::string& name, const char* vShaderfile, const char* fShaderfile, const char* gShaderfile = nullptr);
	static Shader getShader(const std::string& name);
	
	static Texture2D loadTexture(const char* textureFile, bool alpha, const std::string& name);
	static Texture2D getTexture(const std::string& name);

	static void clearResource();

private:
	
	ResourceManager() {}; // we don't want get any resourceManager obj, we just use its packed methods
	static Shader loadShadersFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(const char* textureFile, bool alpha);

};
