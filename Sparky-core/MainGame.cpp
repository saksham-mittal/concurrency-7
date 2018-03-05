#include "MainGame.h"
#include <ArrowsIoEngine\Errors.h>
#include <ArrowsIoEngine/ResourceManager.h>
#define PI 3.14159265

MainGame::MainGame(int noOfPlayers, int currentIndex, const std::vector<Player>& players, socketClient* client) :
	_time(0.0f),
	_screenWidth(1024),
	_screenHeight(768),
	_gameState(GameState::PLAY),
	_maxFPS(120.0f),
	m_currentLevel(0),
	socket(client)
{
	_camera.init(_screenWidth, _screenHeight); 
	m_playerDim = glm::vec2(30.0f, 45.0f);
	m_bulletDim = glm::vec2(15.0f, 15.0f);
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
	//std::cout <<"client receiving -- "<< in<<std::endl;
	mtx.lock();
	data = std::string(in);
	mtx.unlock();
	//}
}


void MainGame::run()
{
	initSystems();


	//_playerTexture = ImageLoader::loadPNG("Textures/PNG/CharacterRight_Standing.png");
	char d[1000];
	strcpy_s(d, m_mainPlayer->getData().c_str());
	newBulls = "0|" + newBulls;
	strcat_s(d, newBulls.c_str());
	socket->sendBytes(d);
	newBulls = "";
	newBullCount = 0;


	gameLoop();
}

/*void MainGame::upDownControl()
{
	if (_inputManager.isKeyDown(SDLK_UP))
		m_mainPlayer->moveUP();

	if (_inputManager.isKeyDown(SDLK_DOWN))
		m_mainPlayer->moveDOWN();
}

void MainGame::rightLeftControl()
{
	if (_inputManager.isKeyDown(SDLK_LEFT))
		m_mainPlayer->moveLEFT();

	if (_inputManager.isKeyDown(SDLK_RIGHT))
		m_mainPlayer->moveRIGHT();
}*/

void MainGame::initSystems()
{
	ArrowsIoEngine::init();

	_window.create("Client", _screenWidth, _screenHeight, 0);

	initShaders();

	for (int i = 0; i < 20; i++)
	{
		_hearts.emplace_back(i);
	}

	_spriteBatch.init();

	initialiseLevel(m_currentLevel);

	_fpsLimiter.init(_maxFPS);

	//m_leveldata = m_levels[m_currentLevel]->getLevelData();
	for (int i = 0; i < m_noOfPlayers; i++)
	{
		m_chars.emplace_back(m_players[i].name, m_players[i].position, m_players[i].playerIndex, m_playerDim, 1/*, m_leveldata*/);
	}

	_heartTexID = ResourceManager::getTexture("../Sparky-core/Textures/Health.png").id;
	_wandTexID = ResourceManager::getTexture("../Sparky-core/Textures/wand.png").id;
	_redTexID = ResourceManager::getTexture("../Sparky-core/Textures/red.png").id;
	_blueTexID = ResourceManager::getTexture("../Sparky-core/Textures/blue.png").id;
	_grayTexID = ResourceManager::getTexture("../Sparky-core/Textures/gray.png").id;

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

		receiver();

		_inputManager.update();
		if (m_mainPlayer->getLife())
			processInput();
		else
			GameState::EXIT;
		_time += 0.01;

		_camera.setPosition(m_mainPlayer->getPosition());
		_camera.update();

		updateNoPlayer();
		updateChars();
		updateBullets();
		updateHearts();

		/*for (int i = 0; i < _bullets.size();)
		{
			if (_bullets[i].update() == true) {
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			}
			else
			{
				i++;
			}
		}*/

		drawGame();

		char d[1000];
		strcpy_s(d, m_mainPlayer->getData().c_str());
		newBulls = /*"0|" + */std::to_string(newBullCount) + "|" + newBulls;
		strcat_s(d, newBulls.c_str());
		//std::cout << d << std::endl;
		socket->sendBytes(d);
		//std::cout << newBulls << std::endl;
		newBulls = "";
		newBullCount = 0;

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
	const float SCALE_SPEED = 0.1f;

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

	if (_inputManager.isKeyPressed(SDL_BUTTON_LEFT)) {
		glm::vec2 mouseCoords = _inputManager.getMouseCoords();
		mouseCoords = _camera.convertScreenToWorld(mouseCoords);

		glm::vec2 direction = mouseCoords - m_mainPlayer->getPosition();
		direction = glm::normalize(direction);
		//std::cout << mouseCoords.x << " , " << mouseCoords.y << std::endl;

		std::string stringPath = "../Sparky-core/Textures/Arrows/";
		int no;
		if (direction.x >= 0 && direction.y >= 0) {
			no = 90 - (int)(atan(direction.y / direction.x) * 180 / PI);
		}
		if (direction.x <= 0 && direction.y >= 0) {
			no = 270 - (int)(atan(direction.y / direction.x) * 180 / PI);
		}
		if (direction.x >= 0 && direction.y <= 0) {
			no = 90 - (int)(atan(direction.y / direction.x) * 180 / PI);
		}
		if (direction.x <= 0 && direction.y <= 0) {
			no = 270 - (int)(atan(direction.y / direction.x) * 180 / PI);
		}
		stringPath += (std::to_string(no) + ".png");
		 ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
		_bullets.emplace_back(m_mainPlayer->getPosition(), direction, texture.id, 10.0f, 1000, m_currentIndex, 1);
		newBulls += _bullets[_bullets.size() - 1].getData();
		newBullCount++;

		//_bullets.emplace_back(m_mainPlayer->getPosition(), direction, 10.0f, 1000);
	}	
	
	if (_inputManager.isKeyDown(SDLK_w))
		m_mainPlayer->moveUP();

	if (_inputManager.isKeyDown(SDLK_s))
		m_mainPlayer->moveDOWN();

	if (_inputManager.isKeyDown(SDLK_a))
		m_mainPlayer->moveLEFT();

	if (_inputManager.isKeyDown(SDLK_d))
		m_mainPlayer->moveRIGHT();

	if (_inputManager.isKeyDown(SDLK_q))
		_camera.setScale(_camera.getScale() + SCALE_SPEED);
	if (_inputManager.isKeyDown(SDLK_e))
		_camera.setScale(_camera.getScale() - SCALE_SPEED);

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

	m_levels[m_currentLevel]->draw();

	_spriteBatch.begin();

	// Drawing characters of clients
	for (int i = 0; i < m_noOfPlayers; i++)
	{
		if(m_chars[i].getLife())
			m_chars[i].draw(_spriteBatch);
	}

	for (int i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_spriteBatch);
	}

	for (int i = 0; i < _hearts.size(); i++)
	{
		if (_hearts[i].getVisiblity())
			_hearts[i].draw(_spriteBatch);
	}


	int health = m_mainPlayer->getHealth();
	float mana = 100.0f;
	_heartPos = _camera.convertScreenToWorld(glm::vec2(40.0f, 40.0f));
	for (int i = 0; i < health; i++)
	{
		_spriteBatch.draw(glm::vec4(_heartPos.x + i *( _heartDim.x + 1.5f), _heartPos.y, _heartDim.x, _heartDim.y), _uv, _heartTexID, 5, _color);
	}
	/*_spriteBatch.draw(glm::vec4(_heartPos.x, _heartPos.y, _heartDim.x, _heartDim.y), _uv, _heartTexID, 5, _color);
	_spriteBatch.draw(glm::vec4(_heartPos.x + 1.5*_heartDim.x, _heartPos.y, health / 4, _heartDim.y), _uv, _redTexID, 5, _color);
	_spriteBatch.draw(glm::vec4(_heartPos.x + 1.5*_heartDim.x + health / 4, _heartPos.y, 50.0f - health / 4, _heartDim.y), _uv, _grayTexID, 5, _color);
	_spriteBatch.draw(glm::vec4(_heartPos.x, _heartPos.y - 2 * _heartDim.y, _heartDim.x, _heartDim.y), _uv, _wandTexID, 5, _color);
	_spriteBatch.draw(glm::vec4(_heartPos.x + 1.5*_heartDim.x, _heartPos.y - 2 * _heartDim.y, mana / 2, _heartDim.y), _uv, _blueTexID, 5, _color);
	_spriteBatch.draw(glm::vec4(_heartPos.x + 1.5*_heartDim.x + mana / 2, _heartPos.y - 2 * _heartDim.y, 50.0f - mana / 2, _heartDim.y), _uv, _grayTexID, 5, _color);*/

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);
	_colorProgram.unuse();

	_window.swapBuffer();
}

void MainGame::updateChars()
{
	mtx.lock();
	std::string tempData = data;
	mtx.unlock();
	//std::cout << "tempData in Client : " << tempData << std::endl;
	//std::cout << "updateChars called in Client." << std::endl;
	if (tempData == "")
	{
		//m_mainPlayer->update();
		return;
	}
	int i = 0;

	for (int j = 0; j < m_noOfPlayers; j++)
	{
		std::string temp = "";
		//x-coordinate
		while (tempData[i] != ' ')
		{
			temp += tempData[i];
			i++;
		}
		float x = std::stof(temp);

		//y-coordinate
		i++;
		temp = "";
		while (tempData[i] != '|')
		{
			temp += tempData[i];
			i++;
		}
		float y = std::stof(temp);

		//health
		i++;
		temp = "";
		while (tempData[i] != '|')
		{
			temp += tempData[i];
			i++;
		}
		int health = std::stoi(temp);

		//getting the health taken
		i++;
		temp = "";
		while (tempData[i] != '|')
		{
			temp += tempData[i];
			i++;
		}
		int healthToPop = std::stoi(temp);
		if (_hearts.size() && healthToPop != -1)
		{
			_hearts[healthToPop].setVisiblity(false);
		}

		//no. Of Bullets
		temp = "";
		i++;
		while (tempData[i] != '|')
		{
			temp += tempData[i];
			i++;
		}
		int numBull = std::stoi(temp);

		//bullet DATA
		temp = "";
		i++;
		for (int z = 0; z < numBull; z++)
		{
			//id of the person shooting
			while (tempData[i] != '|')
			{
				temp += tempData[i];
				i++;
			}
			int pID = std::stoi(temp);

			//bulletType
			i++;
			temp = "";
			while (tempData[i] != '|')
			{
				temp += tempData[i];
				i++;
			}
			int bType = std::stoi(temp);

			//x position
			i++;
			temp = "";
			while (tempData[i] != ' ')
			{
				temp += tempData[i];
				i++;
			}
			float xP = std::stof(temp);

			//y position
			i++;
			temp = "";
			while (tempData[i] != '|')
			{
				temp += tempData[i];
				i++;
			}
			float yP = std::stof(temp);

			//x-direction
			i++;
			temp = "";
			while (tempData[i] != ' ')
			{
				temp += tempData[i];
				i++;
			}
			float xD = std::stof(temp);

			//y direction

			i++;
			temp = "";
			while (tempData[i] != '|')
			{
				temp += tempData[i];
				i++;
			}
			float yD = std::stof(temp);

			i++;
			temp = "";

			std::string stringPath = "../Sparky-core/Textures/Arrows/";
			int no;
			if (xD >= 0 && yD > 0) {
				no = 90 - (int)(atan(yD / xD) * 180 / PI);
			}
			if (xD <= 0 && yD >= 0) {
				no = 270 - (int)(atan(yD / xD) * 180 / PI);
			}
			if (xD >= 0 && yD <= 0) {
				no = 90 - (int)(atan(yD / xD) * 180 / PI);
			}
			if (xD <= 0 && yD <= 0) {
				no = 270 - (int)(atan(yD / xD) * 180 / PI);
			}
			stringPath += (std::to_string(no) + ".png");
			ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
			if (pID != m_currentIndex)
				_bullets.emplace_back(glm::vec2(xP, yP), glm::vec2(xD, yD), /*m_bulletTexID[bType]*/texture.id, 10.0f, 1000, pID, bType);
		}
		if (j != m_currentIndex)
			m_chars[j].setData(x, y, health/* score*/);
	}
	//m_mainPlayer->update();
}

void MainGame::updateBullets()
{
	for (int j = 0; j < m_noOfPlayers; j++)
	{
		for (unsigned int i = 0; i < _bullets.size(); )
		{
			glm::vec2 bulPos = _bullets[i].getPosition();
			glm::vec2 playerPos = m_chars[j].getPosition();
			if (_bullets[i].getPlayerID() == j)
			{
				i++;
				continue;
			}
			if (abs(bulPos.x - playerPos.x) < (m_playerDim.x / 2 + m_bulletDim.x / 2) &&
				abs(bulPos.y - playerPos.y) < (m_playerDim.y / 2 + m_bulletDim.y / 2))
			{
				if (m_chars[j].damageTaken(_bullets[i].getDamage(), livePlayers))
				{
				/*if (m_bullets[i].getPlayerID() == m_currentIndex)
				m_mainPlayer->increaseScore();*/
				}
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				continue;
			}
			if (_bullets[i].update(/*m_leveldata*/))
			{
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			}
			else
				i++;
		}
	}
}

void MainGame::updateHearts()
{
	for (int i = 0; i < _hearts.size(); i++)
	{
		float diff_x = abs(_hearts[i].getPosition().x * 20.0f - m_chars[m_currentIndex].getPosition().x);
		float diff_y = abs(_hearts[i].getPosition().y * 20.0f - m_chars[m_currentIndex].getPosition().y);
		if (diff_x <= 25.0f && diff_y <= 25.0f && _hearts[i].getVisiblity())
		{
			m_mainPlayer->setHeart(i);
			m_mainPlayer->increaseHealth();
			break;
		}

		else
			m_mainPlayer->setHeart(-1);

	}


	for (int i = 0; i < _hearts.size(); i++)
	{
		if (_hearts[i].getVisiblity() == false)
		{
			_hearts[i].updateTimer();
		}
	}
}


void MainGame::initialiseLevel(int level)
{
	m_levels.push_back(new Level("../Sparky-core/Levels/level" + std::to_string(level + 1) + ".txt", _screenWidth, _screenHeight));
}


void MainGame::updateNoPlayer()
{
	int count = 0;
	for (int i = 0; i < m_chars.size(); i++)
	{
		if (m_chars[i].getLife())
			count++;
	}
	livePlayers = count;
}

