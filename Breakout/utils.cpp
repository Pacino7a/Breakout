#include "utils.h"
#include <tuple>
#include <random>

namespace GameUtils
{
	void framebufferSizeCallBack(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

		g_framebuffer_width = width;
		g_framebuffer_height = height;
	}

	bool checkCollision(const GameObj& one, const GameObj& two) // AABB
	{
		bool collisionX{ one.m_position.x + one.m_size.x >= two.m_position.x &&
						 two.m_position.x + two.m_size.x >= one.m_position.x };
		bool collisionY{ one.m_position.y + one.m_size.y >= two.m_position.y && // down is bigger
						 two.m_position.y + two.m_size.y >= one.m_position.y }; // up is smaller
		
		//std::cout << std::boolalpha << "X: " << collisionX << ", Y: " << collisionY << '\n';

		return collisionX && collisionY;
	}

	Collision checkCollisionCircle(const Ball& one, const GameObj& two) // circle and AABB (More precise)
	{
		// get center point circle first 
		glm::vec2 center(one.m_position + one.m_radius);
		// calculate AABB info (center, half-extents)
		glm::vec2 aabb_half_extents(two.m_size.x / 2.0f, two.m_size.y / 2.0f);
		glm::vec2 aabb_center(
			two.m_position.x + aabb_half_extents.x,
			two.m_position.y + aabb_half_extents.y
		);
		// get difference vector between both centers
		glm::vec2 difference = center - aabb_center;
		glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
		// add clamped value to AABB_center and we get the value of box closest to circle
		glm::vec2 closest = aabb_center + clamped;
		// calculate the closest distance between circle and aabb box
		difference = closest - center;
		
		if (glm::length(difference) <= one.m_radius)
			return std::make_tuple(true, vectorDir(difference), difference);
		else
			return std::make_tuple(false, MAX_DIR, glm::vec2(0.0f, 0.0f));

	}

	Directions vectorDir(const glm::vec2& targetTo)
	{
		constexpr std::array compass{ std::to_array<glm::vec2>({{0.0, 1.0}, {0.0, -1.0}, {-1.0, 0.0}, {1.0, 0.0}}) };
		float max{};
		std::size_t bestMatch{ std::numeric_limits<std::size_t>::max() };
		for (std::size_t i{ 0 }; i < compass.size(); ++i)
		{
			float dotProduct{ glm::dot(glm::normalize(targetTo), compass[i]) };
			if (dotProduct > max)
			{
				max = dotProduct;
				bestMatch = i;
			}
		}

		return static_cast<Directions>(bestMatch);
	}

	void activatePowerUp(const PowerUp& powerUp)
	{
		const auto& type{ powerUp.m_type };
		std::cout << "Type: " << type << '\n';
		if (type == "speed")
		{
			ball->m_velocity *= 1.2f;
		}
		else if (type == "sticky")
		{
			ball->m_sticky = true;
			player->m_color = glm::vec3(1.0f, 0.5f, 1.0f);
		}
		else if (type == "passThrough")
		{
			ball->m_passThrough = true;
			ball->m_color = glm::vec3(1.0f, 0.5f, 0.5f);
		}
		else if (type == "sizeIncrease" && player->m_size.x <= PLAYER_SIZE.x * 2)
		{
			player->m_size.x += 50;
		}
		else if (type == "confuse")
		{
			if (!effects->m_confuse)
				effects->m_confuse = true;
		}
		else if (type == "chaos")
		{
			if (!effects->m_chaos)
				effects->m_chaos = true;
		}

	}

	std::unique_ptr<SpriteRender> spriteRender; // <DEFINITION>
	std::unique_ptr<GameObj> player;
	std::unique_ptr<Ball> ball;
	std::unique_ptr<ParticleGen> particles;
	std::unique_ptr<PostProcessor> effects;
	std::unique_ptr<TextRender> text;
	
	irrklang::ISoundEngine* audioEngine{};
	float shakeTime{};
}


Game::Game(unsigned int width, unsigned int height)
	: m_width{ width }, m_height{ height }, m_keyPressed{ false },
	m_state{ GameUtils::GAME_START }, m_level{ 0 }, m_playerLives{ 3 }
{
	init();
}

Game::~Game()
{
	GameUtils::audioEngine->drop(); // only need drop this manually
}

void Game::init()
{
	// if texture's outcome is wrong, first check whether the alpha channel is true!
	ResourceManager::loadTexture("./textures/background.jpg", false, "background");
	ResourceManager::loadTexture("./textures/block_solid.png", false, "solidBrick"); // this png has no Alpha Channel!
	ResourceManager::loadTexture("./textures/block.png", false, "Brick");
	ResourceManager::loadTexture("./textures/paddle.png", true, "Paddle");
	ResourceManager::loadTexture("./textures/particle.png", true, "particle");
	ResourceManager::loadTexture("D:/Cpp/textures/awesomeface.png", true, "face");
	/* Power UP Textures */
	ResourceManager::loadTexture("./textures/powerup_speed.png", true, "speed");
	ResourceManager::loadTexture("./textures/powerup_sticky.png", true, "sticky");
	ResourceManager::loadTexture("./textures/powerup_passthrough.png", true, "passThrough");
	ResourceManager::loadTexture("./textures/powerup_increase.png", true, "sizeIncrease");
	ResourceManager::loadTexture("./textures/powerup_confuse.png", true, "confuse");
	ResourceManager::loadTexture("./textures/powerup_chaos.png", true, "chaos");

	GameLevel level1{}; level1.load("./levels/level1.lvl", m_width, m_height / 2); // we control level height half of screen
	GameLevel level2{}; level2.load("./levels/level2.lvl", m_width, m_height / 2);
	GameLevel level3{}; level3.load("./levels/level3.lvl", m_width, m_height / 2);
	GameLevel level4{}; level4.load("./levels/level4.lvl", m_width, m_height / 2);


	m_levels.emplace_back(level1);
	m_levels.emplace_back(level2);
	m_levels.emplace_back(level3);
	m_levels.emplace_back(level4);


	ResourceManager::loadShader("sprite", "./shaders/Sprite.vert", "./shaders/Sprite.frag");
	ResourceManager::loadShader("particle", "./shaders/Particle.vert", "./shaders/Particle.frag");
	ResourceManager::loadShader("postprocess", "./shaders/Postprocess.vert", "./shaders/Postprocess.frag");

	glm::mat4 projection{ glm::ortho(0.0f, static_cast<float>(m_width),
						  static_cast<float>(m_height), 0.0f, -1.0f, 1.0f) };
	// build spriteShader
	const auto& sprite_shader{ ResourceManager::getShader("sprite") };
	sprite_shader.use();
	sprite_shader.setInt("spriteTexture", 0);
	sprite_shader.setMat4("projection", projection);
	// build particleShader
	const auto& particle_shader{ ResourceManager::getShader("particle") };
	particle_shader.use();
	particle_shader.setInt("spriteTexture", 0);
	particle_shader.setMat4("projection", projection);

	// set SpriteRender
	GameUtils::spriteRender = std::make_unique<SpriteRender>(sprite_shader); // remember this RAII method
	// set PlayerObject (Paddle)
	glm::vec2 playerPos{ m_width / 2 - GameUtils::PLAYER_SIZE.x / 2,
						 m_height - GameUtils::PLAYER_SIZE.y };
	GameUtils::player = std::make_unique<GameObj>(playerPos, GameUtils::PLAYER_SIZE, ResourceManager::getTexture("Paddle"));
	// set BallObject
	auto offset{ glm::vec2{ GameUtils::PLAYER_SIZE.x / 2 - GameUtils::BALL_RADIUS,
						    -(2.0 * GameUtils::BALL_RADIUS)} };
	glm::vec2 ballPos{ playerPos + offset };
	GameUtils::ball = std::make_unique<Ball>(ballPos, GameUtils::BALL_RADIUS,
					   GameUtils::INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
	// set Particles
	GameUtils::particles = std::make_unique<ParticleGen>(particle_shader, ResourceManager::getTexture("particle"),
														 GameUtils::PARTICLE_NR);

	// set Effects
	GameUtils::effects = std::make_unique<PostProcessor>(ResourceManager::getShader("postprocess"), m_width, m_height);

	// add some audios
	GameUtils::audioEngine = irrklang::createIrrKlangDevice();
	GameUtils::audioEngine->play2D("./audios/breakOut.mp3", true);

	// add Text
	GameUtils::text = std::make_unique<TextRender>(m_width, m_height);
	GameUtils::text->load("./fonts/OCRAEXT.TTF", GameUtils::FONT_SIZE);
}

void Game::makeCollisons() // detect collision and reverse the direction of the ball (if 
{
	for (auto& box : m_levels[m_level].m_bricks)
	{
		//if (!box.isSolid() && !box.isDestroyed() && GameUtils::checkCollision(box, *GameUtils::ball)) // AABB
		if (!box.isDestroyed()) // Circle and AABB
		{
			auto collision{ GameUtils::checkCollisionCircle(*GameUtils::ball, box) };// <collided, ballTo, collisionDepth>
			if (std::get<0>(collision)) 
			{
				if(!box.isSolid())
				{
					box.m_destroyed = true;
					GameUtils::audioEngine->play2D("./audios/bleep.mp3", false);
					spawnPowerUps(box);
				}
				else
				{
					GameUtils::shakeTime = 0.05f;
					GameUtils::effects->m_shake = true;
					GameUtils::audioEngine->play2D("./audios/solid.wav", false);

				}
				auto direction{ std::get<1>(collision) };
				auto differenceVec{ std::get<2>(collision) };
				if(box.isSolid() || !GameUtils::ball->m_passThrough)// Bounce only take place when the box is SOLID or the ball is NOT `inevitable`
				{
					if (direction == GameUtils::LEFT || direction == GameUtils::RIGHT)
					{
						GameUtils::ball->m_velocity.x = -GameUtils::ball->m_velocity.x;
						// relocate
						float penetration = GameUtils::ball->m_radius - std::abs(differenceVec.x); // backward
						if (direction == GameUtils::LEFT)
							GameUtils::ball->m_position.x += penetration;
						else
							GameUtils::ball->m_position.x -= penetration;
					}
					else
					{
						GameUtils::ball->m_velocity.y = -GameUtils::ball->m_velocity.y;
						float penetration = GameUtils::ball->m_radius - std::abs(differenceVec.y); // backward
						if (direction == GameUtils::UP) // This `UP` is (0.0, 1.0). In Fact(NDC Coordinates SYS), it's DOWN
							GameUtils::ball->m_position.y -= penetration;
						else
							GameUtils::ball->m_position.y += penetration;
					}
				}
				
			}
			
		}
	}

	// check whether the ball collided with Player (Paddle)
	auto collisonWithPlayer{ GameUtils::checkCollisionCircle(*GameUtils::ball, *GameUtils::player) };
	if (!GameUtils::ball->m_stucked && std::get<0>(collisonWithPlayer))
	{
		auto playerPos{ GameUtils::player->m_position };
		auto playerSize{ GameUtils::player->m_size };
		auto ballPos{ GameUtils::ball->m_position };
		auto ballRadius{ GameUtils::ball->m_radius };
		auto& ballVelocity{ GameUtils::ball->m_velocity };

		float centerBoard{ playerPos.x + playerSize.x / 2.0f };
		float distance{ (ballPos.x + ballRadius) - centerBoard }; // distance between the ball center and Paddel center in Horizontal
		float percentage{ distance / (playerSize.x / 2.0f) }; // the further the ball's center away from the paddle center, the bigger strength should be 
		float strength{ 2.0f };

		ballVelocity.x = GameUtils::INITIAL_BALL_VELOCITY.x * percentage * strength;
		//ballVelocity.y = -ballVelocity.y;
		ballVelocity.y = -1.0f * std::abs(ballVelocity.y);
		auto oldVelocity{ ballVelocity };
		ballVelocity = glm::normalize(ballVelocity) * glm::length(oldVelocity);

		GameUtils::ball->m_stucked = GameUtils::ball->m_sticky; // if the Paddle is sticky, we stuck the ball on the paddle.
		GameUtils::audioEngine->play2D("./audios/bleep.wav", false);
	}

	// check PowerUp's collision with Paddle
	for (auto& powerUp : m_powerUps)
	{
		if (!powerUp.isDestroyed())
		{
			if (powerUp.m_position.y > m_height)
				powerUp.m_destroyed = true;

			if (GameUtils::checkCollision(*GameUtils::player, powerUp))
			{
				GameUtils::activatePowerUp(powerUp);
				powerUp.m_destroyed = true;
				powerUp.m_active = true;
				GameUtils::audioEngine->play2D("./audios/powerup.wav", false);
			}
		}
	}
}


void Game::render()
{
	//GameUtils::spriteRender->drawSprite(ResourceManager::getTexture("face"),
	//	glm::vec2(0.0f, 0.0f), glm::vec2(300.0f, 400.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);

	if (m_state == GameUtils::GAME_START ||
		m_state == GameUtils::GAME_ACTIVE || m_state == GameUtils::GAME_MENU ||
		m_state == GameUtils::GAME_CHAPTER_ENDS || m_state == GameUtils::GAME_WIN)
	{
		// prepare to draw to MSFBO
		GameUtils::effects->preparation();
			/* render scene */
			// draw backGround
			GameUtils::spriteRender->drawSprite(ResourceManager::getTexture("background"),
				glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height));
			// draw level (ALL GameObjects will use spriteRender)
			m_levels[m_level].draw(*GameUtils::spriteRender);
			// draw player (paddle)
			GameUtils::player->draw(*GameUtils::spriteRender);
			// draw powerUps
			for (const auto& pw : m_powerUps)
			{
				if (!pw.isDestroyed())
					pw.draw(*GameUtils::spriteRender);
			}
			// draw particles
			GameUtils::particles->renderParticles();
			// draw ball
			GameUtils::ball->draw(*GameUtils::spriteRender);

		/* blit MSFBO, store rendering result to FBO's texture and render it to Screen */
		GameUtils::effects->render2FBO();
		GameUtils::effects->render2SCR(glfwGetTime());

		// render Text
		std::stringstream lives;
		lives << m_playerLives;
		GameUtils::text->renderText("Lives: " + lives.str(), 5.0f, 5.0f, 1.0f);
	}

	if (m_state == GameUtils::GAME_START)
	{
		GameUtils::text->renderText("welcome to BREAKOUT!", 250.0, m_height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
		GameUtils::text->renderText("Press SPACE to START or ESC to quit", 130.0, m_height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
	}

	if (m_state == GameUtils::GAME_MENU)
	{
		GameUtils::text->renderText("Press SPACE to start", 250.0f, m_height / 2, 1.0f);
		GameUtils::text->renderText("Press W or S to select level", 245.0f, m_height / 2 + 20.0f, 0.75f);
	}

	if (m_state == GameUtils::GAME_WIN)
	{
		GameUtils::text->renderText("Congratulations, You Beat the GAME!", 150.0, m_height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
		GameUtils::text->renderText("Press SPACE to retry or ESC to quit", 130.0, m_height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
	}

	if (m_state == GameUtils::GAME_CHAPTER_ENDS)
	{
		GameUtils::text->renderText("Great, You have finnished this roll!", 150.0, m_height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
		GameUtils::text->renderText("Press SPACE to continue or ESC to quit", 130.0, m_height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
	}
}

void Game::processInput(float deltatime)
{
	if (m_state == GameUtils::GAME_MENU)
	{
		inMenu();
		/* Single Trigger */
		if (!m_keyPressed[GLFW_KEY_SPACE] && m_keys[GLFW_KEY_SPACE])
		{
			m_state = GameUtils::GAME_ACTIVE;
			m_keyPressed[GLFW_KEY_SPACE] = true;
		}
		if (!m_keyPressed[GLFW_KEY_W] && m_keys[GLFW_KEY_W])
		{
			m_level = (m_level + 1) % 4; // (0~3) level0 ~ level3
			m_keyPressed[GLFW_KEY_W] = true;
		}
		if (!m_keyPressed[GLFW_KEY_S] && m_keys[GLFW_KEY_S])
		{
			if (m_level > 0)
				--m_level;
			else
				m_level = 3;
			m_keyPressed[GLFW_KEY_S] = true;
		}
	}

	if (m_state == GameUtils::GAME_ACTIVE)
	{
		float velocity{ GameUtils::PLAYER_VELOCITY * deltatime };
		/* Contiunous Trigger */
		if (m_keys[GLFW_KEY_A])
		{
			if (GameUtils::player->m_position.x > 0.0)
			{
				GameUtils::player->m_position.x -= velocity;
				if (GameUtils::ball->m_stucked)
					GameUtils::ball->m_position.x -= velocity;
			}
		}
		else if (m_keys[GLFW_KEY_D])
		{
			if (GameUtils::player->m_position.x < m_width - GameUtils::player->m_size.x)
			{
				GameUtils::player->m_position.x += velocity;
				if (GameUtils::ball->m_stucked)
					GameUtils::ball->m_position.x += velocity;
			}
		}

		/* Single Trigger */
		if (!m_keyPressed[GLFW_KEY_SPACE] && GameUtils::ball->m_stucked && m_keys[GLFW_KEY_SPACE]) // break stuck
		{
			GameUtils::ball->m_stucked = false;
			m_keyPressed[GLFW_KEY_SPACE] = true; // we you relase the key, this will become false
		}

		if (!m_keyPressed[GLFW_KEY_R] && m_keys[GLFW_KEY_R])
		{
			resetPlayer();
			m_keyPressed[GLFW_KEY_R] = true;
		}
	}

	if (m_state == GameUtils::GAME_START || m_state == GameUtils::GAME_CHAPTER_ENDS)
	{
		if (m_keys[GLFW_KEY_SPACE])
		{
			m_keyPressed[GLFW_KEY_SPACE] = true;
			m_state = GameUtils::GAME_ACTIVE;
		}
	}

	if (m_state == GameUtils::GAME_WIN)
	{
		if (m_keys[GLFW_KEY_SPACE])
		{
			m_keyPressed[GLFW_KEY_SPACE] = true;
			GameUtils::effects->m_chaos = false;
			m_state = GameUtils::GAME_MENU;
		}
	}



}

void Game::resetLevel()
{
	m_levels[m_level].reset();
	m_playerLives = 3;
}

void Game::resetPlayer()
{
	auto xOffset{ GameUtils::player->m_position.x +
	(GameUtils::player->m_size.x / 2 - GameUtils::ball->m_radius) };
	auto yOffset{ GameUtils::player->m_position.y -
		(2 * GameUtils::ball->m_radius) };
	GameUtils::ball->reset(glm::vec2{ xOffset,yOffset }, GameUtils::INITIAL_BALL_VELOCITY);
	// also disable all active powerups
	GameUtils::effects->m_chaos = GameUtils::effects->m_confuse = false;
	GameUtils::ball->m_passThrough = GameUtils::ball->m_sticky = false;
	GameUtils::player->m_color = glm::vec3(1.0f);
	GameUtils::ball->m_color = glm::vec3(1.0f);
}

void Game::update(float deltatime)
{
	// ball move
	GameUtils::ball->move(deltatime, m_width); // this width is the Game's Width
	// do collisions
	makeCollisons();
	// particles
	GameUtils::particles->updateParticles(deltatime, 4, *GameUtils::ball, glm::vec2{ GameUtils::ball->m_radius / 2.0f });
	//update powerUps
	updatePowerUps(deltatime);
	// create some SHAKES
	if (GameUtils::shakeTime > 0.0f)
		GameUtils::shakeTime -= deltatime;
	else
		GameUtils::effects->m_shake = false;
	// check Ball lose
	if (GameUtils::ball->m_position.y >= m_height)
	{
		--m_playerLives;
		if (!m_playerLives)
		{
			resetLevel();
			m_state = GameUtils::GAME_MENU;
		}
		//resetLevel(); // may be too hard?
		resetPlayer();
	}

	if (m_state == GameUtils::GAME_ACTIVE && m_levels[m_level].isCompleted())
	{
		m_powerUps.clear();
		resetPlayer();
		if (m_level == m_levels.size() - 1)
		{
			m_level = 0;
			m_state = GameUtils::GAME_WIN;
			GameUtils::effects->m_chaos = true;
		}
		else
		{
			++m_level;
			m_state = GameUtils::GAME_CHAPTER_ENDS;
		}
		resetLevel();

	}

}

void Game::spawnPowerUps(const GameObj& block) // spawn one UP at once
{
	int random{ Random::get(1,100) };

	if (random <= 15)
		m_powerUps.emplace_back(PowerUp{ "speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.m_position, // duration == 0.0f means inf
			ResourceManager::getTexture("speed") });
	else if (random <= 30)
		m_powerUps.emplace_back(PowerUp{ "sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.m_position,
			ResourceManager::getTexture("sticky") });
	else if (random <= 45)
		m_powerUps.emplace_back(PowerUp{ "passThrough", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.m_position,
			ResourceManager::getTexture("passThrough") });
	else if (random <= 60)
		m_powerUps.emplace_back(PowerUp{ "sizeIncrease", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.m_position,
			ResourceManager::getTexture("sizeIncrease") });
	else if (random <= 80); // spawn Nothing 20%
	else if (random <= 90)
		m_powerUps.emplace_back(PowerUp{ "confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.m_position,
			ResourceManager::getTexture("confuse") });
	else
		m_powerUps.emplace_back(PowerUp{ "chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.m_position,
			ResourceManager::getTexture("chaos") });

}


bool Game::isPowerUpExisted(const std::string& type)
{
	for (const auto& powerUp : m_powerUps)
	{
		if (powerUp.m_active && type == powerUp.m_type) // if there is an another same type is active, we delay to disable the powerUp
			return true;
	}

	return false;
}

void Game::updatePowerUps(float dt)
{
	for (auto& powerUp : m_powerUps)
	{
		powerUp.m_position += powerUp.m_velocity * dt;
		if (powerUp.m_active)
		{
			powerUp.m_duration -= dt;

			if (powerUp.m_duration <= 0.0f)
			{
				powerUp.m_active = false;
				const auto& type{ powerUp.m_type };
				if ( type == "sticky")
				{
					if(!isPowerUpExisted("sticky")) // only off this power when there is no This kind of power left.
					{
						GameUtils::ball->m_sticky = false;
						GameUtils::player->m_color = glm::vec3(1.0f);
					}
				}
				else if(type == "passThrough")
				{
					if (!isPowerUpExisted("passThrough"))
					{
						GameUtils::ball->m_passThrough = false;
						GameUtils::ball->m_color = glm::vec3(1.0f);
					}
				}
				else if(type == "confuse")
				{
					if (!isPowerUpExisted("confuse"))
						GameUtils::effects->m_confuse = false;
				}
				else if(type == "chaos")
				{
					if (!isPowerUpExisted("chaos"))
					{
						GameUtils::effects->m_chaos = false;
					}
				}
			}
		}
	}

	// erase every Non-Active powerUp
	m_powerUps.erase(std::remove_if(m_powerUps.begin(), m_powerUps.end(), [](const PowerUp& p)
		{ return p.isDestroyed() && !p.m_active; }), m_powerUps.end());
}

void Game::inMenu()
{
	// reset ALL levels
	for (auto& level : m_levels)
	{
		level.reset();
	}

	// restore all effect and powerup
	m_powerUps.clear();
	GameUtils::ball->m_sticky = false;
	GameUtils::ball->m_velocity = GameUtils::INITIAL_BALL_VELOCITY;
	GameUtils::ball->m_passThrough = false;
	GameUtils::player->m_size = GameUtils::PLAYER_SIZE;
	GameUtils::effects->m_chaos = false;
	GameUtils::effects->m_confuse = false;

}