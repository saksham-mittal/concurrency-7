#include "MainGameServer.h"
#include <ArrowsIoEngine\Errors.h>
#include <ArrowsIoEngine/ResourceManager.h>

MainGameServer::MainGameServer(int noOfPlayers, int currentIndex, const std::vector<Player>& players, socketServer* server) :
	_time(0.0f),
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameStateServer::PLAY),
	_maxFPS(120.0f),
	socket(server)
{
	_camera.init(_screenWidth, _screenHeight);
	m_playerDim = glm::vec2(30.0f, 30.0f);
	m_bulletDim = glm::vec2(10.0f, 10.0f);
	m_noOfPlayers = noOfPlayers;
	m_currentIndex = currentIndex;
	m_players = players;
}

MainGameServer::~MainGameServer()
{
}

void MainGameServer::receiver()
{
	//while (m_gameState != GameStateServer::EXIT)
	//{
	std::string in;
	socket->receiveData(in);
	std::cout <<"in"<< in<<std::endl;
	while (in[0] == 'i')				//the server reads from local data and hence is not a blocking call. We introduce while loop to prevent game running with initial data indicated by 'i'
		socket->receiveData(in);
	mtx.lock();
	data = std::string(in);
	std::cout << data << std::endl;
	mtx.unlock();
	//}
}

void MainGameServer::run()
{
	initSystems();


	//_playerTexture = ImageLoader::loadPNG("Textures/PNG/CharacterRight_Standing.png");
	std::string strData = m_mainPlayer->getData() + "0|";
	socket->sendData(strData);

	gameLoop();
}

void MainGameServer::upDownControl()
{
	if (_inputManager.isKeyPressed(SDLK_UP))
		m_mainPlayer->moveUP();

	if (_inputManager.isKeyPressed(SDLK_DOWN))
		m_mainPlayer->moveDOWN();
}

void MainGameServer::rightLeftControl()
{
	if (_inputManager.isKeyPressed(SDLK_LEFT))
		m_mainPlayer->moveLEFT();

	if (_inputManager.isKeyPressed(SDLK_RIGHT))
		m_mainPlayer->moveRIGHT();
}

void MainGameServer::initSystems()
{
	ArrowsIoEngine::init();

	_window.create("Server", _screenWidth, _screenHeight, 0);

	initShaders();

	_spriteBatch.init();

	_fpsLimiter.init(_maxFPS);

	//m_leveldata = m_levels[m_currentLevel]->getLevelData();
	for (int i = 0; i < m_noOfPlayers; i++)
	{
		m_chars.emplace_back(m_players[i].name, m_players[i].position, m_players[i].playerIndex, m_playerDim, 1/*, m_leveldata*/);
	}


	m_mainPlayer = &(m_chars[m_currentIndex]);
}

void MainGameServer::initShaders() {
	_colorProgram.compileShaders("../Sparky-core/Shaders/colorShading.vert", "../Sparky-core/Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void MainGameServer::gameLoop()
{
	while (_gameState != GameStateServer::EXIT)
	{
		//used for frame time measuring
		_fpsLimiter.begin();
		receiver();

		processInput();
		_time += 0.01;

		_camera.setPosition(m_mainPlayer->getPosition());
		_camera.update();

		for (int i = 0; i < _bullets.size();)
		{
			if (_bullets[i].update() == true) {
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			}
			else
			{
				i++;
			}
		}

		drawGame();

		std::string strData = m_mainPlayer->getData() + "2|1";
		socket->sendData(strData);

		_fps = _fpsLimiter.end();

		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10000)
		{
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

	}
}

void MainGameServer::processInput()
{
	SDL_Event evnt;

	const float CAMERA_SPEED = 2.0f;
	const float SCALE_SPEED = 0.1f;

	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameStateServer::EXIT;
			break;

		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.pressKey(evnt.key.keysym.sym);
			break;

		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(evnt.button.button);
			break;

		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;

		}
	}

	if (_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
		glm::vec2 mouseCoords = _inputManager.getMouseCoords();
		mouseCoords = _camera.convertScreenToWorld(mouseCoords);

		glm::vec2 playerPosition(0.0f);
		glm::vec2 direction = mouseCoords - playerPosition;
		direction = glm::normalize(direction);
		//std::cout << mouseCoords.x << " , " << mouseCoords.y << std::endl;

		_bullets.emplace_back(playerPosition, direction, 1.0f, 1000);
	}

	if (_inputManager.isKeyPressed(SDLK_s))
		m_mainPlayer->moveUP();

	if (_inputManager.isKeyPressed(SDLK_w))
		m_mainPlayer->moveDOWN();

	if (_inputManager.isKeyPressed(SDLK_d))
		m_mainPlayer->moveLEFT();

	if (_inputManager.isKeyPressed(SDLK_a))
		m_mainPlayer->moveRIGHT();

	if (_inputManager.isKeyPressed(SDLK_q))
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	if (_inputManager.isKeyPressed(SDLK_e))
		_camera.setScale(_camera.getScale() - SCALE_SPEED);

}

void MainGameServer::drawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_colorProgram.use();
	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = _colorProgram.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);


	// Set the camera matrix
	GLint pLocation = _colorProgram.getUniformLocation("P");
	glm::mat4 cameraMatrix = _camera.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	_spriteBatch.begin();
	
	// Drawing characters of clients
	for (int i = 0; i < m_noOfPlayers; i++)
	{
		m_chars[i].draw(_spriteBatch);
	}

	for (int i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_spriteBatch);
	}

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);
	_colorProgram.unuse();

	_window.swapBuffer();
}

