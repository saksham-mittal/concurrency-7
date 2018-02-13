#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include "GLTexture.h"
#include "GLSLProgram.h"
#include <vector>
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
	void calculateFPS();
	SDL_Window * _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	
	std::vector<sprite*> _sprites;

	GLSLProgram _colorProgram;

	float _time;
	float _fps;
	float _maxFPS;
	float _frameTime;
};

