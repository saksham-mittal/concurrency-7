#include "Sockets.h"
#include <ArrowsIoEngine/Errors.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

using namespace ArrowsIoEngine;

Socket::Socket() {

}
Socket::~Socket() {
}

Socket::Socket(size_t MAX_SIZE)
{
	MAX_BUFFER_SIZE = MAX_SIZE;
}

sockError::sockError(const std::string & message) {
#ifdef _WIN32
	fatalErrorSocket(message + std::string(" | sys error: ") + std::to_string(WSAGetLastError()));
#else // !_WIN32
	std::runtime_error(message + std::string(" | sys error: ") + std::strerror(errno));
#endif // _WIN32
}

void socketClient::receiveBytes(char *buffer)
{
	if (buffer == nullptr)
		return;
	long res = -1;
	while (res == -1)
		res = recv(ConnectSocket, buffer, MAX_BUFFER_SIZE, 0);

	if (res == NET_SOCKET_ERROR)
	{
		fatalErrorSocket("Failed to send! (NET_SOCKET_ERROR)");
	}

}

void socketServer::sendBytes(SOCKET & clientSocket, char *buffer)
{
	if (buffer == nullptr)
		return;
	long res = send(clientSocket, buffer, strlen(buffer), 0);
	if (res == NET_SOCKET_ERROR)
	{
		closesocket(clientSocket);
		WSACleanup();
		fatalErrorSocket("Failed to send! (NET_SOCKET_ERROR)");
	}
	std::cout << strlen((char *)buffer) << "Bytes sent" << std::endl;

}

void socketClient::sendBytes(char *buffer)
{
	if (buffer == nullptr)
		return;
	long res = send(ConnectSocket, buffer, strlen(buffer), 0);

	if (res == NET_SOCKET_ERROR)
	{
		fatalErrorSocket("Failed to send! (NET_SOCKET_ERROR)");
	}

}

socketClient::socketClient(std::string &host, std::string & port, size_t MAX_BUFFER_SIZE) : Socket(MAX_BUFFER_SIZE)
{
	if (host.empty())
	{
		std::cout << "Host should not be empty" << std::endl;
	}
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(WSAGetLastError());
	}
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(WSAGetLastError());
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		exit(WSAGetLastError());
	}

}

socketServer::socketServer(std::string  & port, int MAX_CONN, Socket::ConnectionType connType, size_t MAX) : Socket(MAX)
{
	max_clients = MAX_CONN;
	connected_clients = MAX_CONN;
	init = true;

	spawnPositions += std::to_string(respawnPosition[max_clients % 4].x);
	spawnPositions += " ";
	spawnPositions += std::to_string(respawnPosition[max_clients % 4].y);
	spawnPositions += "&";

	for (int i = 0; i < max_clients; i++)
	{
		spawnPositions += std::to_string(respawnPosition[i % 4].x);
		spawnPositions += " ";
		spawnPositions += std::to_string(respawnPosition[i % 4].y);
		spawnPositions += "&";
	}



	clients = new SOCKET[max_clients];
	for (int i = 0; i < max_clients; i++)
		clients[i] = 0;
	collectedData.resize(max_clients);
	std::fill(collectedData.begin(), collectedData.end(), "-1");
	flag.resize(max_clients);
	std::fill(flag.begin(), flag.end(), false);
	buffer = new char[MAX_BUFFER_SIZE];

	struct addrinfo *result = NULL;
	struct addrinfo hints;


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	int iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(iResult);
	}

	// Create a SOCKET for connecting to server
	master = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if ((master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	if (connType == Socket::ConnectionType::NonBlocking)
	{
#ifdef _WIN32
		u_long arg = 1;
		ioctlsocket(master, FIONBIO, &arg);
#else // !_WIN32
		fcntl(socketId, F_SETFL, O_NONBLOCK);
#endif // _WIN32
	}

	if (bind(master, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	// Bind server to the port and start listening at it:
	listen(master, 5);
}

void socketServer::receiveData(std::string & s)
{
	mtx.lock();
	s = serverData;
	mtx.unlock();
}

void socketServer::sendData(std::string & s)
{
	mtxPlayerData.lock();
	playerData = s;
	mtxPlayerData.unlock();
}

void socketServer::select_activity()
{
	int activity, addrlen, i, valread;
	SOCKET s, new_socket;

	std::string msg = "Connected to Server \0 \n";
	char *message = new char[msg.length() + 1];
	strcpy(message, msg.c_str());
	
	addrlen = sizeof(struct sockaddr_in);
	while (connected_clients > 0)
	{
		//clear the socket fd set
		FD_ZERO(&readfds);

		//add master socket to fd set
		FD_SET(master, &readfds);

		//add child sockets to fd set
		for (i = 0; i < max_clients; i++)
		{
			s = clients[i];
			if (s > 0)
			{
				FD_SET(s, &readfds);
			}
		}

		//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		activity = select(0, &readfds, NULL, NULL, NULL);

		if (activity == SOCKET_ERROR)
		{
			printf("select call failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master, &readfds))
		{
			if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			//send new connection greeting message
			sendBytes(new_socket, message);

			puts("Welcome message sent successfully");

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				if (clients[i] == 0)
				{
					clients[i] = new_socket;
					printf("Adding to list of sockets at index %d \n", i);
					break;
				}
			}
		}

		//else its some IO operation on some other socket :)
		for (i = 0; i < max_clients; i++)
		{
			if (clients[i] == 0)
				continue;
			s = clients[i];
			//if client presend in read sockets             
			if (FD_ISSET(s, &readfds))
			{
				//get details of the client
				getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

				//Check if it was for closing , and also read the incoming message
				//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
				valread = recv(s, buffer, MAX_BUFFER_SIZE, 0);

				if (valread == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						//Somebody disconnected , get his details and print
						printf("Host disconnected unexpectedly , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

						//Close the socket and mark as 0 in list for reuse
						closesocket(s);
						clients[i] = 0;
						flag[i] = true;
						connected_clients--;
					}
					else
					{
						printf("recv failed with error code : %d", error_code);
					}
				}
				else if (valread == 0)
				{
					//Somebody disconnected , get his details and print
					printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					closesocket(s);
					clients[i] = 0;
					connected_clients--;
					flag[i] = true;
				}

				//Echo back the message that came in
				else
				{
					//add null character, if you want to use with printf/puts or other string handling functions
					flag[i] = true;
					buffer[valread] = '\0';
					collectedData[i] = std::string(buffer);

				}
			}
		}
		if (std::all_of(flag.begin(), flag.end(), [](bool v) { return v; }))
		{
			mtxPlayerData.lock();
			std::string collectedString = playerData;
			mtxPlayerData.unlock();
			for (int i = 0; i < max_clients; i++)
				collectedString += collectedData[i];
			if (init)
			{
				collectedString = "i" + (std::to_string(max_clients + 1) + "|") + collectedString + spawnPositions + "|";
				playerData = spawnPositions.substr(0, 19) + "|200|0|0|";
				mtx.lock();
				serverData = collectedString;
				mtx.unlock();
				init = false;
			}
			//std::cout << "collected" << collectedString << std::endl;
			for (int i = 0; i < max_clients; i++)
			{
				if (clients[i] != 0)
				{
					mtx.lock();
					serverData = collectedString;
					mtx.unlock();
					long res = send(clients[i], collectedString.c_str(), strlen(collectedString.c_str()), 0);

					if (res == NET_SOCKET_ERROR)
					{
						closesocket(clients[i]);
						WSACleanup();
						fatalErrorSocket("Failed to send! (NET_SOCKET_ERROR)");
					}
				}
			}
			for (int i = 0; i < max_clients; i++)
			{
				if (clients[i] != 0)
					flag[i] = false;
			}
			//std::cout << "Sent data packets to every client" << std::endl;
		}

	}

}