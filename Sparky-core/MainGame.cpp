#include "MainGame.h"
#include <ArrowsIoEngine\Errors.h>
#include <ArrowsIoEngine/ResourceManager.h>
#include <omp.h>
#define PI 3.14159265

MainGame::MainGame(int noOfPlayers, int currentIndex, const std::vector<Player>& players, socketClient* client) :	//initialzing the constructor
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

//function to recive the server data
void MainGame::receiver()
{
	char in[1000];
	socket->receiveBytes(in);
	mtx.lock();
	data = std::string(in);
	mtx.unlock();
}

//main run loop for the game
void MainGame::run()
{
	initSystems();
	char d[1000];
	//sending the initial player info to the server
	strcpy_s(d, m_mainPlayer->getData().c_str());
	newBulls = "0|" + newBulls;
	strcat_s(d, newBulls.c_str());
	socket->sendBytes(d);
	newBulls = "";
	newBullCount = 0;

	//main game loop
	gameLoop();
}

void MainGame::initSystems()
{
	ArrowsIoEngine::init();		//initializing game engine

	_window.create("Arrows.Io", _screenWidth, _screenHeight, 0);	//creating a window

	initShaders();	//initializing shaders

	for (int i = 0; i < 24; i++)		//loop for position of hearts
	{
		_hearts.emplace_back(i);
	}

	_spriteBatch.init();	//initializing the sprite batch

	initialiseLevel(m_currentLevel);		//initializing level

	m_leveldata = m_levels[m_currentLevel]->getLevelData();

	_fpsLimiter.init(_maxFPS);		//capping the max fps

	for (int i = 0; i < m_noOfPlayers; i++)		// creating players
	{
		m_chars.emplace_back(m_players[i].name, m_players[i].position, m_players[i].playerIndex, m_playerDim, 1, m_leveldata);
	}

	_heartTexID = ResourceManager::getTexture("../Sparky-core/Textures/Health.png").id;

	m_mainPlayer = &(m_chars[m_currentIndex]);		//pointer to main player 
}

//function to initialize shaders
void MainGame::initShaders() {
	_colorProgram.compileShaders("../Sparky-core/Shaders/colorShading.vert", "../Sparky-core/Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

//main game loop
void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)		//while the player is in playing mode
	{
		//used for frame time measuring
		_fpsLimiter.begin();

		receiver();		//receving the server data

		_inputManager.update();			//updating the input manager


		if (m_mainPlayer->getLife())
			processInput();				//processing the input given by the player
		else
		{
			SDL_Quit();
		}

		_time += 0.01;

		_camera.setPosition(m_mainPlayer->getPosition());		//setting camera to keep main player in center of the screen
		_camera.update();

		//functions to update diffrent attributes of the game 
		updateChars();
		updateBullets();
		updateHearts();
		updateLive();
		updateNoPlayer();

		//fucntion to draw the game
		drawGame();

		//sending the updated player data to server
		char d[1000];
		strcpy_s(d, m_mainPlayer->getData().c_str());
		newBulls = std::to_string(newBullCount) + "|" + newBulls;
		strcat_s(d, newBulls.c_str());
		socket->sendBytes(d);
		newBulls = "";
		newBullCount = 0;

		_fps = _fpsLimiter.end();

		//printing the fps in 10000 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10000)
		{
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

	}
}

//fuction to process the input given by the player
void MainGame::processInput()
{
	SDL_Event evnt;

	const float CAMERA_SPEED = 2.0f;
	const float SCALE_SPEED = 0.1f;

	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)		//recognizing the event type
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

		if (m_mainPlayer->getGunType() == 1)
		{
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

		}
		else if (m_mainPlayer->getGunType() == 2)
		{
			std::string stringPath = "../Sparky-core/Textures/blade.png";
			ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
			_bullets.emplace_back(m_mainPlayer->getPosition(), direction, texture.id, 4.0f, 300, m_currentIndex, 2);
			_bullets.emplace_back(m_mainPlayer->getPosition(), -direction, texture.id, 4.0f, 300, m_currentIndex, 2);

		}
		else
		{
			std::string stringPath = "../Sparky-core/Textures/circle.png";
			ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
			_bullets.emplace_back(m_mainPlayer->getPosition(), direction, texture.id, 2.0f, 100, m_currentIndex, 3);

		}

		newBulls += _bullets[_bullets.size() - 1].getData();
		newBullCount++;

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

	if (_inputManager.isKeyPressed(SDLK_1))
	{
		m_mainPlayer->setGunType(1);
	}
	if (_inputManager.isKeyPressed(SDLK_2))
	{
		m_mainPlayer->setGunType(2);
	}
	if (_inputManager.isKeyPressed(SDLK_3))
	{
		m_mainPlayer->setGunType(3);
	}

}

//function to draw the game
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
	#pragma omp parallel for
	for (int i = 0; i < m_noOfPlayers; i++)
	{
		if(m_chars[i].getHealth()  > 0)
			m_chars[i].draw(_spriteBatch);
	}

	#pragma omp parallel for
	for (int i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_spriteBatch);
	}

	#pragma omp parallel for
	for (int i = 0; i < _hearts.size(); i++)
	{
		if (_hearts[i].getVisiblity())
			_hearts[i].draw(_spriteBatch);
	}


	int health = m_mainPlayer->getHealth();
	float mana = 100.0f;
	_heartPos = _camera.convertScreenToWorld(glm::vec2(40.0f, 40.0f));

	#pragma omp parallel for
	for (int i = 0; i < health; i++)
	{
		_spriteBatch.draw(glm::vec4(_heartPos.x + i *( _heartDim.x + 1.5f), _heartPos.y, _heartDim.x, _heartDim.y), _uv, _heartTexID, 5, _color);
	}

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	glBindTexture(GL_TEXTURE_2D, 0);
	_colorProgram.unuse();

	_window.swapBuffer();
}

//function to parse the attributes of other players from the server data
void MainGame::updateChars()
{
	mtx.lock();
	std::string tempData = data;
	mtx.unlock();
	if (tempData == "")
	{
		return;
	}
	int i = 0;

	//parse loop
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

			//rendering the bullet according to the bullet type
			if (bType == 1)
			{
				std::string stringPath = "../Sparky-core/Textures/Arrows/";
				int no;
				if (xD >= 0 && yD >= 0) {
					no = 90 - (int)(atan(yD / xD) * 180 / PI);
				}
				if (xD <= 0 && yD >= 0) {
					no = (270 - (int)(atan(yD / xD) * 180 / PI)) % 360;
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
					_bullets.emplace_back(glm::vec2(xP, yP), glm::vec2(xD, yD), texture.id, 10.0f, 1000, pID, bType);
			}
			else if (bType == 2)
			{
				std::string stringPath = "../Sparky-core/Textures/blade.png";
				ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
				if (pID != m_currentIndex)
				{
					_bullets.emplace_back(glm::vec2(xP, yP), glm::vec2(xD, yD), texture.id, 4.0f, 300, pID, bType);
					_bullets.emplace_back(glm::vec2(xP, yP), -glm::vec2(xD, yD), texture.id, 4.0f, 300, pID, bType);
				}
			}
			else
			{
				std::string stringPath = "../Sparky-core/Textures/circle.png";
				ArrowsIoEngine::GLTexture texture = ArrowsIoEngine::ResourceManager::getTexture(stringPath);
				if (pID != m_currentIndex)
					_bullets.emplace_back(glm::vec2(xP, yP), glm::vec2(xD, yD), texture.id, 2.0f, 100, pID, bType);
			}
		}
		if (j != m_currentIndex)
			m_chars[j].setData(x, y, health);
	}
}

//function to update number of live players
void MainGame::updateLive()
{
	for (int i = 0; i < m_chars.size(); i++)
	{
		if (m_chars[i].getHealth() > 0)
			m_chars[i].setLife(true);
		else
			m_chars[i].setLife(false);
	}
}

//function to update the new bullets
void MainGame::updateBullets()
{
	for (int j = 0; j < m_noOfPlayers; j++)
	{
		for (unsigned int i = 0; i < _bullets.size(); )
		{
			glm::vec2 bulPos = _bullets[i].getPosition();
			glm::vec2 playerPos = m_chars[j].getPosition();
			if (_bullets[i].getPlayerID() == j)				//skip the bullet fired by itself
			{
				i++;
				continue;
			}
			if (abs(bulPos.x - playerPos.x) < (m_playerDim.x / 2 + m_bulletDim.x / 2) &&		//condition to hit the bullet
				abs(bulPos.y - playerPos.y) < (m_playerDim.y / 2 + m_bulletDim.y / 2))
			{
				m_chars[j].damageTaken(_bullets[i].getDamage(), livePlayers, m_currentIndex, j);
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				continue;
			}
			if (_bullets[i].update(m_leveldata))		//poping the bullet hitting the walls
			{
				_bullets[i] = _bullets.back();
				_bullets.pop_back();
			}
			else
				i++;
		}
	}
}

//function to update the hearts in the map
void MainGame::updateHearts()
{
	for (int i = 0; i < _hearts.size(); i++)
	{
		float diff_x = abs(_hearts[i].getPosition().x * 45.0f - m_chars[m_currentIndex].getPosition().x);
		float diff_y = abs(_hearts[i].getPosition().y * 45.0f - m_chars[m_currentIndex].getPosition().y);
		if (diff_x <= 25.0f && diff_y <= 25.0f && _hearts[i].getVisiblity())		//if a player takes a heart
		{
			m_mainPlayer->setHeart(i);
			m_mainPlayer->increaseHealth();
			break;
		}

		else
			m_mainPlayer->setHeart(-1);

	}


	for (int i = 0; i < _hearts.size(); i++)		//setting the timer visiblity of the heart
	{
		if (_hearts[i].getVisiblity() == false)
		{
			_hearts[i].updateTimer();
		}
	}
}

//fuction to initialize the level
void MainGame::initialiseLevel(int level)
{
	m_levels.push_back(new Level("../Sparky-core/Levels/level" + std::to_string(level + 1) + ".txt", _screenWidth, _screenHeight));
}

//function for updating false death to true
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

