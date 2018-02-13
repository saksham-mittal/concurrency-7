#include "MainGame.h"
#include "Errors.h"
MainGame::MainGame() : 
	_time(0.0f),
	_screenWidth(1024),
	_screenHeight(768),
	_window(nullptr),
	_gameState(GameState::PLAY)
{

}


MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems();

	_sprites.push_back(new sprite());
	_sprites.back()->init(-1.0f, -1.0f, 1.0f, 1.0f, "Textures/PNG/CharacterRight_Standing.png");

	_sprites.push_back(new sprite());
	_sprites.back()->init(0.0f, -1.0f, 1.0f, 1.0f, "Textures/PNG/CharacterRight_Standing.png");


	//_playerTexture = ImageLoader::loadPNG("Textures/PNG/CharacterRight_Standing.png");

	gameLoop();
}
void MainGame::initSystems()
{
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	_window = SDL_CreateWindow("GameEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
	if (_window == nullptr)
	{
		fatalError("SDL window could not be created");
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
	if (glContext == nullptr)
	{
		fatalError("SDL_GL context could not be created");
	}

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		fatalError("Could not intialize glew!");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	initShaders();
}

void MainGame::initShaders() {
	_colorProgram.compileShaders("Shaders/colorShading.vert", "Shaders/colorShading.frag");
	_colorProgram.addAttribute("vertexPosition");
	_colorProgram.addAttribute("vertexColor");
	_colorProgram.addAttribute("vertexUV");
	_colorProgram.linkShaders();
}

void MainGame::gameLoop()
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		_time += 0.01;
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;

		case SDL_MOUSEMOTION:
			//std::cout << evnt.motion.x << "," << evnt.motion.y << std::endl;
			break;
		}
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

	GLint timeLocation = _colorProgram.getUniformLocation("time");
	glUniform1f(timeLocation, _time);

	for (int i = 0; i < _sprites.size(); i++)
		_sprites[i]->draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	_colorProgram.unuse();

	SDL_GL_SwapWindow(_window);
}
