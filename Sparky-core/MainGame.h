#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <string>

#include <ArrowsIOEngine/GLTexture.h>
#include <ArrowsIOEngine/GLSLProgram.h>

#include <vector>
#include <ArrowsIoEngine/sprite.h>
#include <ArrowsIoEngine\window.h>
#include <ArrowsIoEngine\ArrowsIoEngine.h>
#include <ArrowsIoEngine\Camera2D.h>

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
	ArrowsIoEngine::window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	
	std::vector<ArrowsIoEngine::sprite*> _sprites;

	ArrowsIoEngine::GLSLProgram _colorProgram;
	ArrowsIoEngine::Camera2D _camera;

	float _time;
	float _fps;
	float _maxFPS;
	float _frameTime;
};

