#pragma once
#include <glm\glm.hpp>
#include <ArrowsIoEngine\SpriteBatch.h>
#include <ArrowsIoEngine\ResourceManager.h>
class hearts
{
public:
	hearts(int index);
	~hearts();

	void draw(ArrowsIoEngine::SpriteBatch& spriteBatch);
	void updateTimer();

	void setVisiblity(bool v);
	bool getVisiblity() { return visible; }
	glm::vec2 getPosition() { return _postion + glm::vec2(_dim.x / 2, _dim.y / 2); }	//the dimension have been added to get the position of the centre of the bullet
	glm::vec2 getDim() { return _dim; }
	int getHealth() { return _health; }
	int getHeartIndex() { return _index; }

private:
	glm::vec2 _postion;
	glm::vec2 _dim;
	int _health = 1;
	int _index;
	bool visible = true;
	int _timer = 0;

	glm::vec2 heartPosition[5] = { glm::vec2(100.0f,30.0f),glm::vec2(150.0f,0.0f), glm::vec2(200.0f,50.0f), glm::vec2(-150.0f,-50.0f), glm::vec2(300.0f, -30.0f) };
};

