#include "FTPClient.h"
#include <vector>
#include<sstream>


winSocket::winSocket(const std::string& ip, std::string& port) {
	ConnectSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;

    // Initialize Winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        throw std::runtime_error("WSAStartup failed");
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed");
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket creation failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            throw std::runtime_error("failed to create socket!");
        }

        // Connect to server.
        iResult = ::connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            printf("Trying to connect to the server...\n");
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        throw std::runtime_error("Unable to connect to server!");
    }
    printf("Successfully connected to the server!\n");
}

winSocket::~winSocket() {
    // shutdown the connection
    int iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("socket shutdown failed with error: %d\n", WSAGetLastError());
    }
    closesocket(ConnectSocket);
    WSACleanup();
}

int winSocket::send(const char* sendbuf) {
    // Send an initial buffer

    int iResult = ::send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return -1;
    }
    printf("Bytes Sent: %ld\n", iResult);
    return iResult;
}

std::string winSocket::receive() {
    // Receive until the peer closes the connection
    std::memset(recvbuf, 0x00, sizeof(recvbuf));
    int iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
    } else if (iResult == 0) {
        printf("Connection closed\n");
    } else {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }
    // Print the message
    std::cout << recvbuf << std::endl;
    return std::string(recvbuf, recvbuf + iResult);
}

FTPClient::~FTPClient() {
    if (dataChannel != nullptr) {
        delete dataChannel;
        dataChannel = nullptr;
    }
}

void FTPClient::login() {
    receive();  // Welcome message

    std::string userCMD = "USER " + user;
    executeCommand(userCMD);

    std::string passwdCMD = "PASS " + passwd;
    executeCommand(passwdCMD);
}

void FTPClient::requestNewDataChannel() {
    std::string pasvResponse = executeCommand("PASV");
    std::stringstream s_pasvResponse(pasvResponse);
    std::vector<std::string> split;
    while (s_pasvResponse.good()) {
        std::string substr;
        getline(s_pasvResponse, substr, ',');
        split.push_back(substr);
    }
    std::string port = std::to_string(std::stoi(split[4]) * 256 + std::stoi(split[5]));
    if (dataChannel != nullptr) {
        delete dataChannel;
        dataChannel = nullptr;
    }
    std::cout << "Creating new data channel on " + ip + ":" + port << std::endl;
    dataChannel = new winSocket(ip, port);
}

std::string FTPClient::executeCommand(const std::string& cmd) {
    std::string cmdCRLF = cmd + "\r\n";
    send(cmdCRLF.c_str());
    std::cout << "\033[1;32m" << cmdCRLF << "\033[0m";
    std::string response = receive();
    return response;
}

std::string FTPClient::readData() {
    return dataChannel->receive();
}

void FTPClient::sendData(const std::string& data) {
    dataChannel->send(data.c_str());
}