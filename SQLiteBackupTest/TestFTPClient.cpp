#include <fstream>
#include "TestFTPClient.h"

namespace GT
{
	/**
	 *   @brief in this test case will be tested executeCommand function.
	 *			Performing simple command on the FTP server - make directory "test"
	 *			The result will be compared to the expected message
	 **/
	TEST_F(TestFTPClient, test_execute_command) {
		// FTP user needed to have all permissions on the ftp directory
		// make directory in the FTP server
		std::string mkdir = "MKD test";
		std::string response = pFTP->executeCommand(mkdir);
		std::string expectedResponse = "257 \"/test\" created successfully\r\n";
		ASSERT_EQ(response, expectedResponse);
	}

	/**
	 *   @brief in this test case will be tested read from data channel.
	 *			Performing simple "NLST" command - returns a list of file names in the current directory
	 *			The result will be compared to the expected message
	 **/
	TEST_F(TestFTPClient, test_read_from_data_channel) {
		// Command that requires data channel
		// Returns a list of file names in a specified directory.
		pFTP->requestNewDataChannel();
		pFTP->executeCommand("NLST");
		std::string result = pFTP->readData();

		std::string expectedResult = "sql\r\ntest\r\n";
		ASSERT_EQ(result, expectedResult);
	}

	/**
	 *   @brief in this test case will be tested file transfer to the FTP server.
	 *			"sql" string is the file content to be sent
	 *			"STOR test/test.sql " - Accept the data and store as a file with specified file name
	 *			The command will be executed on command channel and the file will be sent through data channel
	 *			The result of the cmmand form will be comparet to the expected message.
	 **/
	TEST_F(TestFTPClient, test_send_data) {
		std::string sql =
			"PRAGMA foreign_keys = OFF;\n"
			"BEGIN TRANSACTION;\n"
			"CREATE TABLE PERSON(\n"
			"    ID INT PRIMARY KEY     NOT NULL,\n"
			"    NAME           TEXT    NOT NULL,\n"
			"    SURNAME        TEXT    NOT NULL,\n"
			"    AGE            INT     NOT NULL,\n"
			"    ADDRESS        CHAR(50),\n"
			"    SALARY         REAL\n"
			");\n"
			"INSERT INTO PERSON VALUES('1', 'STEVE', 'GATES', '30', 'PALO ALTO', '1000.0');\n"
			"INSERT INTO PERSON VALUES('2', 'BILL', 'ALLEN', '20', 'SEATTLE', '300.22');\n"
			"COMMIT;\n";

		std::string sendFileCMD = "STOR test/test.sql";
		pFTP->requestNewDataChannel();
		std::string status = pFTP->executeCommand(sendFileCMD);
		pFTP->sendData(sql);

		std::string expectedStatus = "150 Opening data channel for file upload to server of \"/test/test.sql\"\r\n";
		ASSERT_EQ(status, expectedStatus);
	}

	/**
	 *   @brief in this test case will be tested change directory command
	 **/
	TEST_F(TestFTPClient, test_change_directory) {
		// Change working directory.
		std::string cd = pFTP->executeCommand("CWD test");
		ASSERT_EQ(cd, "250 CWD successful. \"/test\" is current directory.\r\n");
	}

	/**
	 *   @brief in this test case will be tested list directory command
	 **/
	TEST_F(TestFTPClient, test_list_directory) {
		// Command that requires data channel
		// Returns a list of file names in a specified directory.
		pFTP->requestNewDataChannel();
		pFTP->executeCommand("NLST");
		std::string result = pFTP->readData();

		std::string expectedResult = "test.sql\r\n";
		ASSERT_EQ(result, expectedResult);
	}
}


using namespace GT;

int TestFTPClient::mArgc = 0;
char** TestFTPClient::mArgs = NULL;

FTPClient* TestFTPClient::pFTP = nullptr;

TestFTPClient::TestFTPClient(void) {

}

TestFTPClient::~TestFTPClient(void) {

}

/**
 *   @brief SetUpTestCase() will be called before running the first test in test suite.
 * 
 *   @brief FTPClient object will be created before test suite run
 **/
void TestFTPClient::SetUpTestCase() {
	std::cout << "Set up test cases." << std::endl;

	if (pFTP != NULL) {
		delete pFTP;
	}

	std::string ip = "127.0.0.1";
	std::string port = "21";
	std::string user = "admin";
	std::string passwd = "admin";
	pFTP = new FTPClient(ip, port, user, passwd);
}

/**
 *   @brief TearDownTestCase() will be called after running the last test in test suite.
 * 
 *   @brief will be done cleanup in the FTP server after the test suit run
 **/
void TestFTPClient::TearDownTestCase() {
	std::cout << "Tear down test cases." << std::endl;

	// cleanup test results
	pFTP->executeCommand("DELE test.sql");
	pFTP->executeCommand("CWD ../");
	pFTP->executeCommand("RMD test");

	delete pFTP;
	pFTP = nullptr;
}

void TestFTPClient::SetUp(void) {

}

void TestFTPClient::TearDown(void) {

}


