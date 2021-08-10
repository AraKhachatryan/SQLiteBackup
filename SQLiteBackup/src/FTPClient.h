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

/**
 *   @brief class for data transfer through windows sockets.
 *		    Represents the client side part.
 **/
class winSocket
{
private:
	SOCKET ConnectSocket;

public:
	/**
	 *   @brief constructs an windows socket object.
     *   @param ip - ip address.
     *   @param port - port to connect.
	 **/
	winSocket(const std::string& ip, std::string& port);

	/**
	 *   @brief deoult constructor not available
	 **/
	winSocket() = delete;

	/**
	 *   @brief destructor
	 **/
	virtual ~winSocket();

	/**
	 *   @brief sends the data to the server
	 *   @param sendbuf - data buffer to be sent
	 *   @return returns sent bytes count
	 **/
	int send(const char* sendbuf);

	/**
	 *   @brief reads the data from server and returns as a string
	 **/
	std::string receive();

private:
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
};


/**
 *   @brief class for FTP data transfer inherited from winSocket class.
 **/
class FTPClient: private winSocket
{
private:
	const std::string user;
	const std::string passwd;
	const std::string ip;
	const std::string port;

	/**
	 *   @brief For data channel connection to the FTP server.
	 *          Commands like "LIST" (list directory) and "STOR <filename>" (store the data as a file)
	 *	        requiring data connection to the FTP server on different port
	 **/
	winSocket* dataChannel;

public:
	/**
	 *   @brief Constructs the winSocket base class object through list initialization 
	 *				- used as command channel for FTP server.
	 *	 	    Constructs the FTPClient object.
	 **/
	FTPClient(const std::string& ip, std::string& port, const std::string& user, const std::string& passwd)
		: winSocket(ip, port), user(user), passwd(passwd), ip(ip), port(port) {
		dataChannel = nullptr;
		login();
	}

	FTPClient() = delete;
	virtual ~FTPClient();

	/**
	 *   @brief executes the command through command channel on the FTP server
	 *   @return returns the status/result of the command
	 **/
	std::string executeCommand(const std::string& cmd);

	/**
	 *   @brief requests new data channel connection to the FTP server.
	 *			Algorithm:
	 *				1. executes "PASV" command - entering passive mode
	 *				2. reads the response and calculates the port number to connect
	 *				3. destroys the old dataChannel connection
	 *				4. creates new dataChannel connection with newly calculated port number
	 **/
	void requestNewDataChannel();

	/**
	 *   @brief reads the data from data channel as a string
	 **/
	std::string readData();

	/**
	 *   @brief sends the data trough data channel
	 **/
	void sendData(const std::string& data);

private:
	/**
	 *   @brief performs login to the FTP server
	 **/
	void login();
};

#endif