#include "WinSocket.h"
#include <vector>
#include <sstream>

Win_socket::Win_socket(const std::string& ip, std::string& port) {
    socket_fd = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int res;
    // Initialize Winsock
    WSADATA wsaData;
    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        printf("WSAStartup failed with error: %d\n", res);
        throw std::runtime_error("WSAStartup failed");
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    res = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
    if (res != 0) {
        printf("getaddrinfo failed with error: %d\n", res);
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed");
    }
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (socket_fd == INVALID_SOCKET) {
            printf("socket creation failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            throw std::runtime_error("failed to create socket!");
        }
        // Connect to server.
        res = ::connect(socket_fd, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (res == SOCKET_ERROR) {
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
            printf("Failed to connect to the server...\n");
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if (socket_fd == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Unable to connect to server!");
    }
    printf("Successfully connected to the server!\n");
}

Win_socket::~Win_socket() {
    // shutdown the connection
    int res = shutdown(socket_fd, SD_SEND);
    if (res == SOCKET_ERROR) {
        printf("socket shutdown failed with error: %d\n", WSAGetLastError());
    }
    closesocket(socket_fd);
    WSACleanup();
}

int Win_socket::send(const char* sendbuf) {
    // Send an initial buffer
    int res = ::send(socket_fd, sendbuf, (int)strlen(sendbuf), 0);
    if (res == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(socket_fd);
        WSACleanup();
        return -1;
    }
    printf("Bytes Sent: %ld\n", res);
    return res;
}

std::string Win_socket::receive() {
    std::memset(recvbuf, 0x00, sizeof(recvbuf));
    // Receive the data
    int res = recv(socket_fd, recvbuf, recvbuflen, 0);
    if (res > 0) {
        printf("Bytes received: %d\n", res);
    }
    else if (res == 0) {
        printf("Connection closed\n");
    }
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }
    // Print the message
    std::cout << recvbuf << std::endl;
    return std::string(recvbuf, recvbuf + res);
}