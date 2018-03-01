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

#include <ArrowsIoEngine\SpriteBatch.h>

#include <ArrowsIoEngine\ArrowsIoEngine.h>
#include <ArrowsIoEngine\Camera2D.h>

#include <ArrowsIoEngine\InputManager.h>
#include <ArrowsIoEngine\Timing.h>

#include "Bullet.h"
#include "Sockets.h"
#include <mutex>
enum class GameState { PLAY, EXIT };

class MainGame
{
public:
	MainGame(socketClient* client);
	~MainGame();

	void run();


private:
	void initSystems();
	void initShaders();
	void processInput();
	void receiver();
	void gameLoop();
	void drawGame();
	ArrowsIoEngine::window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	ArrowsIoEngine::GLSLProgram _colorProgram;
	ArrowsIoEngine::Camera2D _camera;

	ArrowsIoEngine::InputManager _inputManager;
	ArrowsIoEngine::FpsLimiter _fpsLimiter;

	ArrowsIoEngine::SpriteBatch _spriteBatch;

	std::vector<Bullet> _bullets;

	float _fps;
	float _maxFPS;
	float _time;

	std::mutex mtx;
	socketClient* socket;
	std::string data;
};

