#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include "GLTexture.h"
#include "GLSLProgram.h"

#include "sprite.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();


private:
	void initSystems();
	void initShaders();
	void processInput();
	void gameLoop();
	void drawGame();
	SDL_Window * _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	sprite _sprite;

	GLTexture _playerTexture;

	GLSLProgram _colorProgram;

	float _time;
};

