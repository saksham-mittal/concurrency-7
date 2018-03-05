#pragma once

#include <glm/glm.hpp>
#include <ArrowsIoEngine\SpriteBatch.h>
#include <string>
class Bullet
{
public:
	Bullet(glm::vec2 pos, glm::vec2 dir, GLuint textureID,float speed, int lifeTime, int playerID, int bulletType);
	~Bullet();

	void draw(ArrowsIoEngine::SpriteBatch& spriteBatch);
	bool update(const std::vector<std::string>& levelData);

	//getters
	glm::vec2 getPosition() { return _postion + glm::vec2(_dim.x / 2, _dim.y / 2); }	//the dimension have been added to get the position of the centre of the bullet
	glm::vec2 getDim() { return _dim; }
	int getPlayerID() { return _playerID; }
	int getDamage();
	std::string getData() { return std::to_string(_playerID) + "|" + std::to_string(_bulletType) + "|" + std::to_string(_postion.x) + " " + std::to_string(_postion.y) + "|" + std::to_string(_direction.x) + " " + std::to_string(_direction.y) + "|"; }

private:
	int _lifeTime;
	float _speed;
	glm::vec2 _direction;
	glm::vec2 _postion;
	glm::vec2 _dim;
	int _playerID;
	int _bulletType;
	int _damage[2] = { 1,2 };
	GLuint _textureID;
};

