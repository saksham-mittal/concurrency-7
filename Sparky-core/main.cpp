#include "MainGame.h"
#include "Player.h"
#include "MainGameServer.h"
#include <ArrowsIoEngine/Errors.h>
#include <iostream>
#include "Sockets.h"
#include <thread>
#include <vector>
#include <string>

using namespace ArrowsIoEngine;

/*int main() {
	using namespace sparky;
	using namespace graphics;
	using namespace maths;


	Window window("Sparky!", 960, 540);
	// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

#if 0
	GLfloat vertices[] = {
		0, 0, 0,
		8, 0, 0,
		0, 3, 0,
		0, 3, 0,
		8, 3, 0,
		8, 0, 0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
#else

	GLfloat vertices[] = {
		0, 0, 0,
		0, 3, 0,
		8, 3, 0,
		8, 0, 0
	};

	GLushort indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLfloat colorsA[] = {
		1, 0, 1, 1,
		1, 0, 1, 1,
		1, 0, 1, 1,
		1, 0, 1, 1
	};

	GLfloat colorsB[] = {
		0.2f, 0.3f, 0.8f, 1,
		0.2f, 0.3f, 0.8f, 1,
		0.2f, 0.3f, 0.8f, 1,
		0.2f, 0.3f, 0.8f, 1,
	};

	VertexArray sprite1, sprite2;
	IndexBuffer ibo(indices, 6);

	sprite1.addBuffer(new Buffer(vertices, 4 * 3, 3), 0);
	sprite1.addBuffer(new Buffer(colorsA, 4 * 4, 4), 1);

	sprite2.addBuffer(new Buffer(vertices, 4 * 3, 3), 0);
	sprite2.addBuffer(new Buffer(colorsB, 4 * 4, 4), 1);

#endif

	mat4 ortho = mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);
	
	Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");
	shader.enable();
	shader.setUniformMat4("pr_matrix", ortho);
	shader.setUniformMat4("ml_matrix", mat4::translation(vec3(4, 3, 0)));

	shader.setUniform2f("light_pos", vec2(4.0f, 1.5f));
	shader.setUniform4f("colour", vec4(0.2f, 0.3f, 0.8f, 1.0f));

	while (!window.closed()) {
		window.clear();
		double x, y;
		window.getMousePosition(x, y);
		shader.setUniform2f("light_pos", vec2((float)(x * 16.0f / 960.0f), (float)(9.0f - y * 9.0f / 540.0f)));
#if 0
		glDrawArrays(GL_TRIANGLES, 0, 6);
#else
		sprite1.bind();
		ibo.bind();
		shader.setUniformMat4("ml_matrix", mat4::translation(vec3(4, 3, 0)));
		glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_SHORT, 0);
		ibo.unbind();
		sprite1.unbind();

		sprite2.bind();
		ibo.bind();
		shader.setUniformMat4("ml_matrix", mat4::translation(vec3(0, 0, 0)));
		glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_SHORT, 0);
		ibo.unbind();
		sprite2.unbind();
#endif
		window.update();
	}
	
	return 0;
}*/

/*int main(int argc, char** argv)
{*/

	int main(int argc, char** argv)
	{
		int noOfPlayers = 0;
		int indexOfClient = 0;
		//std::vector<Player> players;
		char name[100];
		std::cout << "Enter your username\n";
		std::cin >> name;
		std::string name2;
		name2 = std::string(name);
		/*std::string om = "My name is om";
		char temp[10];
		strcpy(temp, om.c_str());*/


		std::cout << "Enter your character choice 0-6\n";
		char playerChoice[100];
		std::cin >> playerChoice;
		strcat_s(name, "|");
		strcat_s(name, playerChoice);
		strcat_s(name, "|");


		int choice;
		std::cout << "Enter 1 to be server,2 to be client\n";
		std::cin >> choice;


		std::string SOCK_PORT = "100";
		std::thread sockThread;

		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 0), &info))
			fatalError("Could not start WSA");

		if (choice == 1)
		{
			int clients;
			std::cout << "Enter number of Clients" << std::endl;
			std::cin >> clients;
			socketServer server(SOCK_PORT, clients, Socket::ConnectionType::NonBlocking, 2048);
			std::string tmp(name);
			std::cout << "Temp:" << tmp << std::endl;
			server.sendData(tmp);
			sockThread = std::thread(&socketServer::select_activity, &server);
			//std::cout << "Going to wait" << std::endl;
			while (server.init);
			//std::cout << "Resumed" << std::endl;
			std::string input = "";
			server.receiveData(input);
			std::cout << " Printing the input :::: " <<  input << std::endl;
			// processString(input, name2, indexOfClient, noOfPlayers, players);
			// SimpleGameServer simpleGame(noOfPlayers, indexOfClient, players, &server);
			MainGameServer mainGame(&server);
			mainGame.run();
			//MainGame mainGame;
			//mainGame.run();
			sockThread.join();
		}
		else if (choice == 2)
		{
			std::cout << "Enter server's IP address\n";
			std::string ip;
			std::cin >> ip;
			socketClient client(ip, SOCK_PORT, 2048);
			char input[1000];
			client.receiveBytes(input);
			std::cout << input;	//connected msg
			//std::cout << name << std::endl;
			client.sendBytes(name);
			//client.sendBytes(name);
			client.receiveBytes(input);
			std::cout << input << std::endl;
			// processString(std::string(input), name2, indexOfClient, noOfPlayers, players);
			// SimpleGame mainGame(noOfPlayers, indexOfClient, players, &client);
			MainGame mainGame;
			mainGame.run();
		}
		/*int x;
		std::cout << "Enter any character to quit" << std::endl;
		std::cin >> x;*/
		return 0;
	}
	/*MainGame mainGame;
	mainGame.run();
	//system("pause");
	return 0;
}*/