#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET NetSocketId;
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define NET_INVALID_SOCKET_ID INVALID_SOCKET
#define NET_SOCKET_ERROR      SOCKET_ERROR

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <mutex>

class sockError {
public:
	sockError(const std::string&  message);
};


class Socket
{
public:
	enum class ConnectionType { Blocking, NonBlocking };
	~Socket();

protected:
	size_t MAX_BUFFER_SIZE;
	fd_set readfds;
	Socket();
	Socket(size_t MAX_BUFFER_SIZE);

};

class socketClient : public Socket {
public:
	socketClient(std::string & host, std::string & port, size_t max_size);
	void sendBytes(char * buffer);
	void receiveBytes(char *receiveBuffer);
private:
	std::string host;
	u_short port;
	SOCKET ConnectSocket;
};


class socketServer : public Socket {
public:
	socketServer(std::string  & port, int MAX_CONN, Socket::ConnectionType connType, size_t MAX);
	void select_activity();
	void sendBytes(SOCKET &newSocket, char *buffer);
	void sendData(std::string& s);
	void receiveData(std::string & s);
	bool init;
private:
	SOCKET master;
	SOCKET * clients;
	int max_clients;
	int connected_clients;
	struct sockaddr_in server, address;
	char * buffer;
	std::vector<std::string> collectedData;
	std::vector<bool> flag;

	std::string spawnPositions;
	glm::vec2 respawnPosition[4] = { glm::vec2(130.0f,75.0f),glm::vec2(250.0f,100.0f), glm::vec2(130.0f,500.0f), glm::vec2(200.0f,600.0f) };
	std::string serverData;
	std::string playerData;
	std::mutex mtx;
	std::mutex mtxPlayerData;
};



