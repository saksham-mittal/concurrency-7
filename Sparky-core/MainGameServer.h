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
#include "Character.h"
#include "Player.h"
#include "Level.h"
#include <mutex>
enum class GameStateServer { PLAY, EXIT };

class MainGameServer
{
public:
	MainGameServer(int noOfPlayers, int currentIndex, const std::vector<Player>& players, socketServer* server);
	~MainGameServer();

	void run();

private:
	void initSystems();
	void initShaders();
	void processInput();
	void receiver();
	void gameLoop();
	void drawGame();
	// void initialiseLevel(int currentLevel);
	void upDownControl();
	void rightLeftControl();
	ArrowsIoEngine::window _window;
	int _screenWidth;
	int _screenHeight;
	GameStateServer _gameState;

	int m_currentLevel;
	std::vector<std::string> m_leveldata;

	ArrowsIoEngine::GLSLProgram _colorProgram;
	ArrowsIoEngine::Camera2D _camera;

	ArrowsIoEngine::InputManager _inputManager;
	ArrowsIoEngine::FpsLimiter _fpsLimiter;

	ArrowsIoEngine::SpriteBatch _spriteBatch;

	std::vector<Bullet> _bullets;
	std::vector<Level*> m_levels;

	float _fps;
	float _maxFPS;
	float _time;

	glm::vec2 m_playerDim, m_bulletDim;
	std::vector<Character> m_chars;
	Character* m_mainPlayer;

	int m_noOfPlayers, m_currentIndex;
	std::vector<Player> m_players;
	std::mutex mtx;
	socketServer* socket;
	std::string data;
};

