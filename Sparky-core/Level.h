#pragma once
#include <string>
#include <vector>
#include <ArrowsIoEngine/SpriteBatch.h>

const int TILE_WIDTH = 45;

class Level
{
public:
	Level(const std::string& fileName, int screenWidth, int screenHeight);
	~Level();

	void draw();
	//getters
	std::vector<std::string> getLevelData() { return m_levelData; }

private:
	std::vector<std::string> m_levelData;
	ArrowsIoEngine::SpriteBatch m_spriteBatch;
	GLuint brick1Id;
	GLuint brick2Id;
	GLuint grassId;
	GLuint floorId;

};
