#include "Bullet.h"
#include <ArrowsIoEngine\ResourceManager.h>
#include "Level.h"


//defining the constructor
Bullet::Bullet(glm::vec2 pos, glm::vec2 dir, GLuint textureID, float speed, int lifeTime, int playerID, int bulletType)
{
	_postion = pos;
	_lifeTime = lifeTime;
	_direction = dir;
	_speed = speed;
	_playerID = playerID;
	_bulletType = bulletType;
	_textureID = textureID;
	if (bulletType == 1)
		_dim = glm::vec2(30.0f, 30.0f);
	else if (bulletType == 2)
		_dim = glm::vec2(25.0f, 25.0f);
	else
		_dim = glm::vec2(20.0f, 20.0f);
}


Bullet::~Bullet()
{
}

//function to draw the bullet
void Bullet::draw(ArrowsIoEngine::SpriteBatch& spriteBatch)
{
	ArrowsIoEngine::Color color;
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	//the dimensions of bullet has been subtracted to centre the bullet
	spriteBatch.draw(glm::vec4(_postion.x - _dim.x / 2, _postion.y - _dim.y / 2, _dim.x, _dim.y), uv, _textureID, 0.0f, color);
}

bool Bullet::update(const std::vector<std::string>& levelData)
{
	_postion += _direction * _speed;
	_lifeTime--;
	if (_lifeTime == 0)
		return true;
	//check colision with a tile
	if (levelData[(_postion.x / TILE_WIDTH)][(_postion.y / TILE_WIDTH)] != '.')
		return true;
	if (levelData[((_postion.x + _dim.x) / TILE_WIDTH)][(_postion.y / TILE_WIDTH)] != '.')
		return true;
	if (levelData[(_postion.x / TILE_WIDTH)][((_postion.y + _dim.y) / TILE_WIDTH)] != '.')
		return true;
	if (levelData[((_postion.x + _dim.x) / TILE_WIDTH)][((_postion.y + _dim.y) / TILE_WIDTH)] != '.')
		return true;
	return false;
}

//funciotn for taking the damage from the bullet type
int Bullet::getDamage()
{
	if (_bulletType == 3)
		return _damage[1];
	else
		return _damage[0];
}
