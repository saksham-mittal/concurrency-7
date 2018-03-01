#include "MainGame.h"
#include <ArrowsIoEngine\Errors.h>
#include <ArrowsIoEngine/ResourceManager.h>

MainGame::MainGame(int noOfPlayers, int currentIndex, const std::vector<Player>& players, socketClient* client) :
	_time(0.0f),
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_maxFPS(120.0f),
	socket(client)
{
	_camera.init(_screenWidth, _screenHeight); 
	m_playerDim = glm::vec2(30.0f, 30.0f);
	m_bulletDim = glm::vec2(10.0f, 10.0f);
	m_noOfPlayers = noOfPlayers;
	m_currentIndex = currentIndex;
	m_players = players;

}


MainGame::~MainGame()
{
}

void MainGame::receiver()
{
	//while (m_gameState != GameState::EXIT)
	//{
	char in[1000];
	socket->receiveBytes(in);
	//std::cout <<"in"<< in<<std::endl;
	mtx.lock();
	data = std::string(in);
	mtx.unlock();
	//}
}


void MainGame::run()
{
	initSystems();


	//_playerTexture = ImageLoader::loadPNG("Textures/PNG/CharacterRight_Standing.png");
	std::string strData = m_mainPlayer->getData() + "0|";
	char data[100];
	strcpy(data, strData.c_str());
	socket->sendBytes(data);


	gameLoop();
}

void MainGame::initSystems()
{
	ArrowsIoEngine::init();

	_window.create("Client", _screenWidth, _screenHeight, 0);

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

void MainGame::initShaders() {
	_colorProgram.compileShaders("../Sparky-core/Shaders/colorShading.vert", "../Sparky-core/Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		//used for frame time measuring
		_fpsLimiter.begin();
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

		std::string strData = m_mainPlayer->getData() + "0|";
		char data[100];
		strcpy(data, strData.c_str());
		socket->sendBytes(data);

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

void MainGame::processInput()
{
	SDL_Event evnt;

	const float CAMERA_SPEED = 2.0f;
	const float SCALE_SPPED = 0.1f;

	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
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


	if (_inputManager.isKeyPressed(SDLK_w)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
	}

	if (_inputManager.isKeyPressed(SDLK_s)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(0.0f, -CAMERA_SPEED));
	}
	
	if (_inputManager.isKeyPressed(SDLK_a)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(-CAMERA_SPEED, 0.0f));
	}
	
	if (_inputManager.isKeyPressed(SDLK_d)) {
		_camera.setPosition(_camera.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}
	
	if (_inputManager.isKeyPressed(SDLK_q)) {
		_camera.setScale(_camera.getScale() + SCALE_SPPED);
	}
	
	if (_inputManager.isKeyPressed(SDLK_e)) {
		_camera.setScale(_camera.getScale() - SCALE_SPPED);
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
	
	
}

void MainGame::drawGame()
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

