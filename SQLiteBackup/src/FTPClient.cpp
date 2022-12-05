#include "FTPClient.h"
#include <vector>
#include <sstream>

FTP_client::~FTP_client() {
    if (data_channel != nullptr) {
        delete data_channel;
        data_channel = nullptr;
    }
}

void FTP_client::login() {
    receive();  // Welcome message
    std::string user_cmd = "USER " + user;
    execute_command(user_cmd);
    std::string passwd_cmd = "PASS " + passwd;
    execute_command(passwd_cmd);
}

void FTP_client::request_new_data_channel() {
    std::string pasv_response = execute_command("PASV");
    std::stringstream s_pasv_response(pasv_response);
    std::vector<std::string> split;
    while (s_pasv_response.good()) {
        std::string substr;
        getline(s_pasv_response, substr, ',');
        split.push_back(substr);
    }
    std::string port = std::to_string(std::stoi(split[4]) * 256 + std::stoi(split[5]));
    if (data_channel != nullptr) {
        delete data_channel;
        data_channel = nullptr;
    }
    std::cout << "Creating new data channel on " + ip + ":" + port << std::endl;
    data_channel = new Win_socket(ip, port);
}

std::string FTP_client::execute_command(const std::string& cmd) {
    std::string cmd_CRLF = cmd + "\r\n";
    send(cmd_CRLF.c_str());
    std::cout << "\033[1;32m" << cmd_CRLF << "\033[0m";
    std::string response = receive();
    return response;
}

std::string FTP_client::read_data() {
    return data_channel->receive();
}

void FTP_client::send_data(const std::string& data) {
    data_channel->send(data.c_str());
}