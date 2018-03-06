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

void processString(std::string & input, std::string & name2, int & indexOfClient, int & noOfPlayers, std::vector<Player> & players);

	int main(int argc, char** argv)
	{
		int noOfPlayers = 0;
		int indexOfClient = 0;
		std::vector<Player> players;		//vector of player attributes
		char name[100];
		std::cout << "Enter your username\n";
		std::cin >> name;
		std::string name2;
		name2 = std::string(name);

		std::cout << "Enter your character choice 0-3\n";
		char playerChoice[100];
		std::cin >> playerChoice;
		strcat_s(name, "|");
		strcat_s(name, playerChoice);
		strcat_s(name, "|");


		int choice;
		std::cout << "Enter 1 to be server,2 to be client\n";
		std::cin >> choice;


		std::string SOCK_PORT = "100";		//port for the server
		std::thread sockThread;

		WSADATA info;	//the windows socket info
		if (WSAStartup(MAKEWORD(2, 0), &info))
			fatalError("Could not start WSA");

		//choice for server
		if (choice == 1)
		{
			int clients;
			std::cout << "Enter number of Clients" << std::endl;
			std::cin >> clients;
			socketServer server(SOCK_PORT, clients, Socket::ConnectionType::NonBlocking, 2048);		//creating the server
			std::string tmp(name);
			server.sendData(tmp);
			sockThread = std::thread(&socketServer::select_activity, &server);		//thread which makes wait the server
			while (server.init);
			std::string input = "";
			server.receiveData(input);
			processString(input, name2, indexOfClient, noOfPlayers, players);
			MainGameServer mainGame(noOfPlayers, indexOfClient, players, &server);		//initalizing game
			mainGame.run();
			sockThread.join();
		}
		else if (choice == 2)		//choice for client
		{
			std::cout << "Enter server's IP address\n";
			std::string ip;
			std::cin >> ip;
			socketClient client(ip, SOCK_PORT, 2048);		//creating the client and starting connection
			char input[1000];
			client.receiveBytes(input);
			std::cout << input;	//connected msg
			client.sendBytes(name);
			client.receiveBytes(input);
			std::cout << input << std::endl;
			std::string temp(input);
			processString(temp, name2, indexOfClient, noOfPlayers, players);
			MainGame mainGame(noOfPlayers, indexOfClient, players, &client);		//starting game
			mainGame.run();
		}

		return 0;
	}

	//function for processing the input string and setting the attributes in the vector of players
	void processString(std::string & input, std::string & name2, int & indexOfClient, int & noOfPlayers, std::vector<Player> & players)
	{
		int i = 1;
		// Data is received in the form no_of_players| followed by
		while (input[i] != '|')
		{
			noOfPlayers = input[i] - '0' + 10 * noOfPlayers;
			i++;
		}
		i++;
		std::string *pname = new std::string[noOfPlayers];
		int *pchoice = new int[noOfPlayers];
		for (int j = 0; j < noOfPlayers; j++)
		{
			pname[j] = "";
			pchoice[j] = 0;
			while (input[i] != '|')
			{
				pname[j] += input[i];
				i++;
			}
			if (name2 == pname[j])
				indexOfClient = j;

			i++;
			std::string temp = "";
			while (input[i] != '|')
			{
				temp += input[i];
				pchoice[j] = std::stoi(temp);
				i++;
			}
			i++;
		}
		glm::vec2 *pos = new glm::vec2[noOfPlayers];

		for (int j = 0; j < noOfPlayers; j++)
		{
			std::string temp = "";
			while (input[i] != ' ')
			{
				temp += input[i];
				i++;
			}
			i++;
			float x = std::stof(temp);
			temp = "";
			while (input[i] != '&')
			{
				temp += input[i];
				i++;
			}
			i++;
			float y = std::stof(temp);
			pos[j] = glm::vec2(x, y);
		}
		for (int j = 0; j < noOfPlayers; j++)
		{
			players.emplace_back(pname[j], pos[j], pchoice[j]);		//pushing individual player data
		}
	}
