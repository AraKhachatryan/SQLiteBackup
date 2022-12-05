#ifndef  WINSOCKET_H
#define  WINSOCKET_H

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

/**
 *   @brief class for data transfer through windows sockets.
 *		    Represents the client side part.
 **/
class Win_socket
{
private:
	SOCKET socket_fd;

public:
	/**  @brief constructs an windows socket object.
	 *   @param ip - ip address.
	 *   @param port - port to connect.
	 **/
	Win_socket(const std::string& ip, std::string& port);

	/**  @brief deoult constructor not available
	 **/
	Win_socket() = delete;

	/**  @brief destructor
	 **/
	virtual ~Win_socket();

	/**  @brief sends the data to the server
	 *   @param sendbuf - data buffer to be sent
	 *   @return returns sent bytes count
	 **/
	int send(const char* sendbuf);

	/**  @brief reads the data from server and returns as a string
	 **/
	std::string receive();

private:
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
};

#endif