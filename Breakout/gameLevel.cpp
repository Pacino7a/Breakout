#include "gameLevel.h"

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
	auto width{ tileData[0].size() };
	auto height{ tileData.size() };
	float unit_width{ static_cast<float>(levelWidth) / static_cast<float>(width) }; // x stride
	float unit_height{ static_cast<float>(levelHeight) / static_cast<float>(height) }; // y stride

	//std::cout << "SCR_width: " << levelWidth << ", SCR_height: " << levelHeight << '\n';
	//std::cout << "data_width: " << width << ", data_height: " << height << '\n';
	//std::cout << "unit_width: " << unit_width << ", unit_height: " << unit_height << '\n';

	//std::cout << "Width: " << width << ", " << "Height: " << height << '\n';
	for (std::size_t row{ 0 }; row < height; ++row)
	{
		for (std::size_t col{ 0 }; col < width; ++col)
		{
			if (tileData[row][col] == 1) // solid brick
			{
				glm::vec2 pos{ col * unit_width, row * unit_height }; // use the stride to locate the obj's position
				glm::vec2 size{ unit_width, unit_height };
				//std::cout << "Pos: " << pos.x << ", " << pos.y << '\n';
				//std::cout << "Scale: " << size.x << ", " << size.y << '\n';
				GameObj obj{ pos, size, ResourceManager::getTexture("solidBrick"), glm::vec3{0.8f, 0.8f, 0.7f} };
				obj.m_isSolid = true;
				m_bricks.emplace_back(std::move(obj));
			}
			else if (tileData[row][col] > 1)
			{
				glm::vec3 color{ 1.0f };
				switch (tileData[row][col])
				{
				case 2:
					color = glm::vec3(0.2f, 0.6f, 1.0f);
					break;
				case 3:
					color = glm::vec3(0.0f, 0.7f, 0.0f);
					break;
				case 4:
					color = glm::vec3(0.8f, 0.8f, 0.4f);
					break;
				case 5:
					color = glm::vec3(1.0f, 0.5f, 0.0f);
					break;
				default:
					break;
				}
				glm::vec2 pos{ col * unit_width, row * unit_height }; // use the stride to locate the obj's position
				glm::vec2 size{ unit_width, unit_height };
				//std::cout << "Pos: " << pos.x << ", " << pos.y << '\n';
				//std::cout << "Scale: " << size.x << ", " << size.y << '\n';
				m_bricks.emplace_back(GameObj{ pos, size, ResourceManager::getTexture("Brick"), color });
			}
		}
	}
}

void GameLevel::load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
	// load level directly from file.
	// flash the bricks' status
	m_bricks.clear();
	GameLevel level;
	std::string line;
	std::ifstream fstream(file); // read the file, file -> fileStream(getline) -> stringStream(seperate by Spaces) -> unsigned int
	std::vector<std::vector<unsigned int>> tileData; // save the level data
	unsigned int tileCode{}; // for Transform String to Integer
	if (fstream)
	{
		for (; std::getline(fstream, line);) // read filestream in (string) line; file -> stringline
		{
			std::istringstream sstream{ line };
			std::vector<unsigned int> row;
			for (; sstream >> tileCode;) // string -> characters (divided by space)
				row.emplace_back(tileCode);
			tileData.emplace_back(std::move(row));
		}
		if (!tileData.empty())
		{
			init(tileData, levelWidth, levelHeight);
		}
	}
}

void GameLevel::draw(const SpriteRender& spriteRender)
{
	for (const auto& tile : m_bricks)
	{
		if (!tile.isDestroyed())
		{
			tile.draw(spriteRender);
		}
	}
}

bool GameLevel::isCompleted() // Game Complete judge
{
	for (const auto& brick : m_bricks)
	{
		if (!brick.isSolid() && !brick.isDestroyed()) // if Non-solid brick has not been Destroyed yet
			return false;
	}

	return true; // if all non-solid bricks passed the checking

}

void GameLevel::reset()
{
	for (auto& brick : m_bricks)
	{
		if (!brick.isSolid() && brick.isDestroyed())
		{
			brick.m_destroyed = false;
		}
	}
}