#include <fstream>
#include "TestFTPClient.h"

namespace GT
{
	/** @brief in this test case will be tested execute_command function.
	 *			Performing simple command on the FTP server - make directory "test"
	 *			The result will be compared to the expected message
	 **/
	TEST_F(Test_FTP_client, test_execute_command) {
		// FTP user needed to have all permissions on the ftp directory
		// make directory in the FTP server
		std::string mkdir = "MKD test";
		std::string response = p_ftp->execute_command(mkdir);
		std::string expected_response = "257 \"/test\" created successfully.\r\n";
		ASSERT_EQ(response, expected_response);
	}

	/** @brief in this test case will be tested read from data channel.
	 *			Performing simple "NLST" command - returns a list of file names in the current directory
	 *			The result will be compared to the expected message
	 **/
	TEST_F(Test_FTP_client, test_read_from_data_channel) {
		// Command that requires data channel
		// Returns a list of file names in a specified directory.
		p_ftp->request_new_data_channel();
		p_ftp->execute_command("NLST");
		std::string result = p_ftp->read_data();
		std::string expected_result = "sql\r\ntest\r\n";
		ASSERT_EQ(result, expected_result);
	}

	/** @brief in this test case will be tested file transfer to the FTP server.
	 *			"sql" string is the file content to be sent
	 *			"STOR test/test.sql" - Accept the data and store as a file with specified file name
	 *			The command will be executed on command channel and the file will be sent through data channel
	 *			The result of the cmmand form will be comparet to the expected message.
	 **/
	TEST_F(Test_FTP_client, test_send_data) {
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
		std::string send_file_cmd = "STOR test/test.sql";
		p_ftp->request_new_data_channel();
		std::string status = p_ftp->execute_command(send_file_cmd);
		p_ftp->send_data(sql);
		std::string expected_status = "150 Starting data transfer.\r\n";
		ASSERT_EQ(status, expected_status);
	}

	/**  @brief in this test case will be tested change directory command
	 **/
	TEST_F(Test_FTP_client, test_change_directory) {
		// Change working directory.
		std::string cd = p_ftp->execute_command("CWD test");
		ASSERT_EQ(cd, "250 CWD command successful\r\n");
	}

	/**  @brief in this test case will be tested list directory command
	 **/
	TEST_F(Test_FTP_client, test_list_directory) {
		// Command that requires data channel
		// Returns a list of file names in a specified directory.
		p_ftp->request_new_data_channel();
		p_ftp->execute_command("NLST");
		std::string result = p_ftp->read_data();
		std::string expected_result = "test.sql\r\n";
		ASSERT_EQ(result, expected_result);
	}
}


using namespace GT;

int Test_FTP_client::mArgc = 0;
char** Test_FTP_client::mArgs = NULL;

FTP_client* Test_FTP_client::p_ftp = nullptr;

Test_FTP_client::Test_FTP_client(void) {

}

Test_FTP_client::~Test_FTP_client(void) {

}

/**  @brief SetUpTestCase() will be called before running the first test in test suite.
 *   @brief FTP_client object will be created before test suite run
 **/
void Test_FTP_client::SetUpTestCase() {
	std::cout << "Set up test cases." << std::endl;
	if (p_ftp != NULL) {
		delete p_ftp;
	}
	std::string ip = "127.0.0.1";
	std::string port = "21";
	std::string user = "admin";
	std::string passwd = "admin";
	p_ftp = new FTP_client(ip, port, user, passwd);
}

/**  @brief TearDownTestCase() will be called after running the last test in test suite.
 *   @brief will be done cleanup in the FTP server after the test suit run
 **/
void Test_FTP_client::TearDownTestCase() {
	std::cout << "Tear down test cases." << std::endl;
	// cleanup test results
	p_ftp->execute_command("DELE test.sql");
	p_ftp->execute_command("CWD ../");
	p_ftp->execute_command("RMD test");
	delete p_ftp;
	p_ftp = nullptr;
}

void Test_FTP_client::SetUp(void) {

}

void Test_FTP_client::TearDown(void) {

}


