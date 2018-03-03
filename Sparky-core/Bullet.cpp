#include "Bullet.h"
#include <ArrowsIoEngine\ResourceManager.h>




Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, GLuint textureID, float speed, int lifeTime, int playerID, int bulletType)
{
	_postion = pos;
	_lifeTime = lifeTime;
	_direction = dir;
	_speed = speed;
	_playerID = playerID;
	_bulletType = bulletType;
	_textureID = textureID;
	_dim = glm::vec2(35.0f, 35.0f);
}


Bullet::~Bullet()
{
}


void Bullet::draw(ArrowsIoEngine::SpriteBatch& spriteBatch)
{
	ArrowsIoEngine::Color color;
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	//static ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture("../Sparky-core/Textures/Arrow.png");
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	/*glm::vec4 posAndSize = glm::vec4(_postion.x, _postion.y, 7, 7);
	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, color);*/


	//the dimensions of bullet has been subtracted to centre the bullet
	spriteBatch.draw(glm::vec4(_postion.x - _dim.x / 2, _postion.y - _dim.y / 2, _dim.x, _dim.y), uv, _textureID, 0.0f, color);
}

bool Bullet::update()
{
	_postion += _direction * _speed;
	_lifeTime--;
	if (_lifeTime == 0)
		return true;
	return false;
}
