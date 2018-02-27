#include "Errors.h"

#include <cstdlib>

#include <iostream>
#include <SDL/SDL.h>

namespace ArrowsIoEngine {

	void fatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to exit" << std::endl;
		int temp;
		std::cin >> temp;
		SDL_Quit();
		exit(1);
	}
	void fatalErrorSocket(std::string errorString)
	{
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to quit" << std::endl;
		int tmp;
		std::cin >> tmp;
		exit(1);
	}
}