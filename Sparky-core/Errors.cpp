#include "Errors.h"

#include <cstdlib>

#include <iostream>
#include <SDL/SDL.h>

void fatalError(std::string errorString)
{
	std::cout << errorString << std::endl;
	std::cout << "Enter any key to exit" << std::endl;
	int temp;
	std::cin >> temp;
	SDL_Quit();
	exit(1);
}
