#pragma once
#include <vector>
#include "gameObj.h"
#include "resourceManager.h"

class GameLevel
{
public:
	explicit GameLevel() {}
	
	std::vector<GameObj> m_bricks{};
	void load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	void draw(const SpriteRender& spriteRender);
	void reset();
	bool isCompleted();

private:
	void init(std::vector<std::vector<unsigned int>> tileData, // aka. level data (write in a txt file. 0 for NONE, 1 for solid, other break-able objs
			  unsigned int levelWidth, unsigned int levelHeight);
};