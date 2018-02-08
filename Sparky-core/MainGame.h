#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();


private:
	void initSystems();
	void processInput();
	void gameLoop();
	void drawGame();
	SDL_Window * _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;
};

