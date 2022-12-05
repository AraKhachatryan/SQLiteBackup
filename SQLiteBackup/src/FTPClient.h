#ifndef  FTPCLIENT_H
#define  FTPCLIENT_H

#include "WinSocket.h"

/**
 *   @brief class for FTP data transfer inherited from Win_socket class.
 **/
class FTP_client: private Win_socket
{
private:
	const std::string user;
	const std::string passwd;
	const std::string ip;
	const std::string port;

	/**  @brief For data channel connection to the FTP server.
	 *          Commands like "LIST" (list directory) and "STOR <filename>" (store the data as a file)
	 *          requiring data connection to the FTP server on different port
	 **/
	Win_socket* data_channel;

public:
	/**  @brief Constructs the Win_socket base class object through list initialization.
	 *          Constructs the FTP_client object.
	 **/
	FTP_client(const std::string& ip, std::string& port, const std::string& user, const std::string& passwd)
			try : Win_socket(ip, port), user(user), passwd(passwd), ip(ip), port(port) {
		data_channel = nullptr;
		this->login();
	}
	catch(const std::runtime_error& error) {
		std::cout << "Error: Socket initialization failed: " << error.what() << std::endl;
		throw;
	}

	FTP_client() = delete;
	virtual ~FTP_client();

	/**  @brief executes the command through command channel on the FTP server
	 *   @return returns the status/result of the command
	 **/
	std::string execute_command(const std::string& cmd);

	/**  @brief requests new data channel connection to the FTP server.
	 *          Algorithm:
	 *              1. executes "PASV" command - entering passive mode
	 *              2. reads the response and calculates the port number to connect
	 *              3. destroys the old data_channel connection
	 *              4. creates new data_channel connection with newly calculated port number
	 **/
	void request_new_data_channel();

	/**  @brief reads the data from data channel as a string
	 **/
	std::string read_data();

	/**  @brief sends the data trough data channel
	 **/
	void send_data(const std::string& data);

private:
	/**  @brief performs login to the FTP server
	 **/
	void login();
};

#endif
