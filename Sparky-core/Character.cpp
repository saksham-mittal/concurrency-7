#include "Character.h"
#include <iostream>

Character::Character(std::string name, glm::vec2 pos, int person, glm::vec2 dim, int speed, const std::vector<std::string>& levelData)
{
	m_health = 7;
	m_name = name;
	m_position = pos;
	m_person = person;
	m_dim = dim;
	//m_texId[NOTSHOOTING] = ResourceManager::getTexture(m_filePaths[m_person]).id;
	//m_texId[SHOOTING] = ResourceManager::getTexture(m_filePaths2[m_person]).id;
	m_speed = speed;
	m_texId = ResourceManager::getTexture(m_filePaths[m_person]).id;
	m_levelData = levelData;
	//m_state = NOTSHOOTING;
	m_score = -1;
	gunID = 1;
}

Character::~Character()
{
}

void Character::setData(float x, float y, int health /*int score*/)
{
	m_position.x = x;
	m_position.y = y;
	m_health = health;
	//m_score = score;
}

void Character::setHeart(int heart)
{
	m_score = heart;
}

void Character::respawn()
{
	m_health = 7;
	int x = rand() % RESPAWN_PLACES;
	m_position = respawnPosition[x];
}

bool Character::damageTaken(int damage, int livePlayers)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		life = false;
		std::cout << m_name << " DEAD and your rank is " << livePlayers  << std::endl;

		//respawn();
		return true;
	}
	return false;
}

void Character::increaseHealth()
{
	if (m_health < 7)
		m_health++;
}
std::string Character::getData()
{
	std::string result = std::to_string(m_position.x) + " " + std::to_string(m_position.y) + "|" + std::to_string(m_health) + "|" + std::to_string(m_score) + "|";
	return result;
}

void Character::draw(SpriteBatch& spriteBatch)
{
	//static GLTexture texture = ResourceManager::getTexture("../Sparky-core/Textures/PNG/CharacterRight_Standing.png");
	spriteBatch.draw(glm::vec4(m_position.x, m_position.y, m_dim.x, m_dim.y), m_uv, m_texId, 0.0f, m_color);
}


void Character::moveUP()
{
	if ((m_levelData[floor(m_position.x / (float)TILE_WIDTH)][ceil((m_position.y + m_dim.y) / (float)TILE_WIDTH)] != '.') ||
		(m_levelData[floor((m_position.x + m_dim.x) / (float)TILE_WIDTH)][ceil((m_position.y + m_dim.y) / (float)TILE_WIDTH)] != '.'))		//wall above somewhere
	{
		int distance = ((int)(m_position.y + m_dim.y)) % TILE_WIDTH;
		if ((TILE_WIDTH - distance) < MIN_WALL_DISTANCE)
			return; //without updating the position, as the player cannot move any closer than the min distance 
	}
	m_position += glm::vec2(0.0f, m_speed);
	return;
}

void Character::moveDOWN()
{
	if ((m_levelData[floor(m_position.x / (float)TILE_WIDTH)][floor((m_position.y) / (float)TILE_WIDTH) - 1] != '.') ||
		(m_levelData[floor((m_position.x + m_dim.x) / (float)TILE_WIDTH)][floor((m_position.y) / (float)TILE_WIDTH) - 1] != '.')) //wall below somewhere
	{
		int distance = ((int)(m_position.y)) % TILE_WIDTH;
		if (distance < MIN_WALL_DISTANCE)
			return; //without updating the position, as the player cannot move any closer than the min distance 
	}
	m_position += glm::vec2(0.0f, -m_speed);
	return;
}

void Character::moveLEFT()
{	
	if ((m_levelData[floor(m_position.x / (float)TILE_WIDTH) - 1][floor((m_position.y + m_dim.y) / (float)TILE_WIDTH)] != '.') ||
		(m_levelData[floor((m_position.x) / (float)TILE_WIDTH) - 1][floor((m_position.y) / (float)TILE_WIDTH)] != '.'))	//wall on the left somehwere
	{
		int distance = ((int)(m_position.x)) % TILE_WIDTH;
		if (distance < MIN_WALL_DISTANCE)
			return; //without updating the position, as the player cannot move any closer than the min distance 
	}
	m_position += glm::vec2(-m_speed, 0.0f);
	return;
}


void Character::moveRIGHT()
{
	if ((m_levelData[ceil((m_position.x + m_dim.x) / (float)TILE_WIDTH)][floor((m_position.y + m_dim.y) / (float)TILE_WIDTH)] != '.') ||
		(m_levelData[ceil((m_position.x + m_dim.x) / (float)TILE_WIDTH)][floor((m_position.y) / (float)TILE_WIDTH)] != '.'))	//wall on the right
	{
		int distance = ((int)(m_position.x + m_dim.x)) % TILE_WIDTH;
		if ((TILE_WIDTH - distance) < MIN_WALL_DISTANCE)
			return;	//without updating the position, as the player cannot move any closer than the min distance 
	}
	m_position += glm::vec2(m_speed, 0.0f);
	return;
}
