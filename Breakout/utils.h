#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <array>
#include <learnGL/random_s_class.h>
#include <irrKlang/irrKlang.h>
#include "spriteRender.h"
#include "resourceManager.h"
#include "gameLevel.h"
#include "gameBall.h"
#include "particle.h"
#include "postProcessor.h"
#include "powerUp.h"
#include "textRender.h"

extern int g_framebuffer_width;
extern int g_framebuffer_height;

namespace GameUtils
{
	enum GameState
	{
		GAME_START,
		GAME_ACTIVE,
		GAME_MENU,
		GAME_CHAPTER_ENDS,
		GAME_WIN,
		MAX_STATE
	};

	enum Directions
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		MAX_DIR
	};

	using Collision = std::tuple<bool, Directions, glm::vec2>;

	void framebufferSizeCallBack(GLFWwindow* window, int width, int height);
	bool checkCollision(const GameObj& one, const GameObj& two);
	Collision checkCollisionCircle(const Ball& one, const GameObj& two);
	Directions vectorDir(const glm::vec2& targetTo);
	void activatePowerUp(const PowerUp& powerUp);

	// Ptr's Statements
	extern std::unique_ptr<SpriteRender> spriteRender; // `extern` is ESSENTIAL for variables in Namespace <STATEMENT>
	extern std::unique_ptr<GameObj> player;
	extern std::unique_ptr<Ball> ball;
	extern std::unique_ptr<ParticleGen> particles;
	extern std::unique_ptr<PostProcessor> effects;
	extern std::unique_ptr<TextRender> text;

	extern irrklang::ISoundEngine* audioEngine;

	// implicit INLINE
	constexpr glm::vec2 PLAYER_SIZE{ 100.0f, 20.0f };
	constexpr glm::vec2 INITIAL_BALL_VELOCITY{ 100.0f, -350.0f };
	constexpr float PLAYER_VELOCITY{ 500.0f };
	constexpr float BALL_RADIUS{ 12.5f };
	constexpr unsigned int PARTICLE_NR{ 500 };
	constexpr unsigned int FONT_SIZE{ 24 };

	extern float shakeTime;
}

class Game
{
public:
	explicit Game(unsigned int width, unsigned int height);
	~Game(); // we have uniqueptr to control Resources, so we don't really need this
			 // but the AUDIO resource can't use uniquePtr, so ..

	std::vector<GameLevel> m_levels{};
	std::vector<PowerUp> m_powerUps{};

	std::size_t m_level{};
	unsigned int m_width{};
	unsigned int m_height{};
	unsigned int m_playerLives{};
	bool m_keys[1024]{};
	bool m_keyPressed[1024]{};
	GameUtils::GameState m_state{};

	void init();
	void processInput(float deltatime);
	void makeCollisons();
	void update(float deltatime);
	void render();
	void resetLevel();
	void resetPlayer();
	void spawnPowerUps(const GameObj& obj);
	void updatePowerUps(float dt);

private:
	bool isPowerUpExisted(const std::string& type);
	void inMenu();

};

