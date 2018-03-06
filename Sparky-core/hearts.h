#pragma once
#include <glm\glm.hpp>
#include <ArrowsIoEngine\SpriteBatch.h>
#include <ArrowsIoEngine\ResourceManager.h>
#include <string>
#include <vector>
#include <fstream>


class hearts
{
public:
	hearts(int index);
	~hearts();

	void draw(ArrowsIoEngine::SpriteBatch& spriteBatch);
	void updateTimer();

	void setVisiblity(bool v);
	bool getVisiblity() { return visible; }
	glm::vec2 getPosition() { return _postion; }	//the dimension have been added to get the position of the centre of the bullet
	glm::vec2 getDim() { return _dim; }
	int getHealth() { return _health; }
	int getHeartIndex() { return _index; }
	
	std::vector <glm::vec2> heartPosition;

private:
	glm::vec2 _postion;
	glm::vec2 _dim;
	int _health = 1;
	int _index;
	bool visible = true;
	int _timer = 0;
	std::vector<std::string> m_levelData;
};

