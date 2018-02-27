#include "Bullet.h"
#include <ArrowsIoEngine\ResourceManager.h>
#

Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, float speed, int lifeTime)
{
	_postion = pos;
	_lifeTime = lifeTime;
	_direction = dir;
	_speed = speed;
}


Bullet::~Bullet()
{
}


void Bullet::draw(ArrowsIoEngine::SpriteBatch& spriteBatch)
{
	ArrowsIoEngine::Color color;
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture("../Sparky-core/Textures/PNG/CharacterRight_Standing.png");
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	glm::vec4 posAndSize = glm::vec4(_postion.x, _postion.y, 30, 30);
	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, color);
}

bool Bullet::update()
{
	_postion += _direction * _speed;
	_lifeTime--;
	if (_lifeTime == 0)
		return true;
	return false;
}
