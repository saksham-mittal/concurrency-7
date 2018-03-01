#include "Character.h"


Character::Character(std::string name, glm::vec2 pos, int person, glm::vec2 dim, int speed/*, const std::vector<std::string>& levelData*/)
{
	//m_health = 200.0f;
	m_name = name;
	m_position = pos;
	//m_person = person;
	m_dim = dim;
	//m_texId[NOTSHOOTING] = ResourceManager::getTexture(m_filePaths[m_person]).id;
	//m_texId[SHOOTING] = ResourceManager::getTexture(m_filePaths2[m_person]).id;
	//m_speed = speed;
	//m_levelData = levelData;
	//m_state = NOTSHOOTING;
}

Character::~Character()
{
}

void Character::setData(float x, float y, float health, int score)
{
	m_position.x = x;
	m_position.y = y;
	//m_health = health;
	//m_score = score;
}


std::string Character::getData()
{
	std::string result = std::to_string(m_position.x) + " " + std::to_string(m_position.y) /*+ "|" + std::to_string(m_health) + "|" + std::to_string(m_score) + "|"*/;
	return result;
}

void Character::draw(SpriteBatch& spriteBatch)
{
	static GLTexture texture = ResourceManager::getTexture("../Sparky-core/Textures/PNG/CharacterRight_Standing.png");
	spriteBatch.draw(glm::vec4(m_position.x, m_position.y, m_dim.x, m_dim.y), m_uv, texture.id, 0.0f, m_color);
}
