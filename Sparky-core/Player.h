#pragma once
#include <string>
#include <glm/glm.hpp>
struct Player
{
	Player(std::string Name, glm::vec2 pos, int index) :name(Name), position(pos), playerIndex(index)
	{
	}
	std::string name;
	int playerIndex;
	glm::vec2 position;
};