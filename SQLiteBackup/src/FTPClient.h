#ifndef  FTPCLIENT_H
#define  FTPCLIENT_H

#define WIN32_LEAN_AND_MEAN

//#include <WinSock2.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 1024

class winSocket
{
private:
	SOCKET ConnectSocket;

public:
	winSocket(const std::string& ip, std::string& port);
	winSocket() = delete;
	virtual ~winSocket();

	int send(const char* sendbuf);
	std::string receive();

private:
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
};

class FTPClient: private winSocket
{
private:
	const std::string user;
	const std::string passwd;
	const std::string ip;
	const std::string port;
	winSocket* dataChannel;

public:
	FTPClient(const std::string& ip, std::string& port, const std::string& user, const std::string& passwd)
		: winSocket(ip, port), user(user), passwd(passwd), ip(ip), port(port) {
		dataChannel = nullptr;
		login();
	}

	FTPClient() = delete;
	virtual ~FTPClient();

	std::string executeCommand(const std::string& cmd);
	void requestNewDataChannel();
	std::string readData();
	void sendData(const std::string& data);

private:
	void login();
};

#endif