#include "hearts.h"
#include <ArrowsIoEngine/Errors.h>

using namespace ArrowsIoEngine;
hearts::hearts(int index)
{
	_index = index;
	_dim = glm::vec2(20.0f, 20.0f);

	//opening the level file
	std::ifstream file;
	file.open("../Sparky-core/Levels/hearts.txt");

	//eeror checking
	if (file.fail())
		fatalError("Failed to open hearts.txt");

	//loading the file data
	std::string line;
	while (std::getline(file, line))
	{
		m_levelData.push_back(line);
	}

	//rendering the heart at its position
	for (int y = 0; y < m_levelData.size(); y++)
	{
		for (int x = 0; x < m_levelData[y].size(); x++)
		{
			if (m_levelData[y][x] == '@') {
				heartPosition.push_back(glm::vec2(y, x));
			}
		}
	}

	_postion = heartPosition[index];

}

hearts::~hearts()
{
}

//function to draw the hearts
void hearts::draw(ArrowsIoEngine::SpriteBatch & spriteBatch)
{
	ArrowsIoEngine::Color color;
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	static GLuint texture = ResourceManager::getTexture("../Sparky-core/Textures/heart.png").id;
	spriteBatch.draw(glm::vec4((_postion.x) * 45, (_postion.y) * 45, _dim.x, _dim.y), uv, texture, 0.0f, color);
}

//function for the visiblity of the heart
void hearts::updateTimer()
{
	if (_timer < 1000)
	{
		_timer++;
	}
	else
	{
		_timer = 0;
		visible = true;
	}
	
}

//the visiblity variable
void hearts::setVisiblity(bool v)
{
	visible = v;
}