#include "hearts.h"

using namespace ArrowsIoEngine;
hearts::hearts(int index)
{
	_index = index;
	_postion = heartPosition[index];
	_dim = glm::vec2(20.0f, 20.0f);
}

hearts::~hearts()
{
}

void hearts::draw(ArrowsIoEngine::SpriteBatch & spriteBatch)
{
	ArrowsIoEngine::Color color;
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	//static ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture("../Sparky-core/Textures/Arrow.png");
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	static GLuint texture = ResourceManager::getTexture("../Sparky-core/Textures/heart.png").id;
	spriteBatch.draw(glm::vec4(_postion.x - _dim.x / 2, _postion.y - _dim.y / 2, _dim.x, _dim.y), uv, texture, 0.0f, color);
}


void hearts::updateTimer()
{
	if (_timer < 500)
	{
		_timer++;
	}
	else
	{
		_timer = 0;
		visible = true;
	}
	
}

void hearts::setVisiblity(bool v)
{
	visible = v;
}
