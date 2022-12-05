#ifndef TESTFTPCLIENT_H
#define TESTFTPCLIENT_H

#include "gtest/gtest.h"
#include "../SQLiteBackup/src/FTPClient.h"

namespace GT
{
	class Test_FTP_client : public ::testing::Test
	{
	public:
		/**  @brief through constructor can be done set-up work for each test.
		 **/
		Test_FTP_client(void);

		/**  @brief through destructor can be done clean-up work for each test.
		 **/
		virtual ~Test_FTP_client(void) override;

		/**  @brief SetUpTestCase() will be called before running the first test in test suite.
		 **/
		static void SetUpTestCase();

		/**  @brief TearDownTestCase() will be called after running the last test in test suite.
		 **/
		static void TearDownTestCase();

		/**  @brief SetUp() will be called immediately after the constructor (right before each test).
		 **/
		virtual void SetUp(void) override;

		/**  @brief TearDown() will be called immediately after each test (right before the destructor).
		 **/
		virtual void TearDown(void) override;

	public:
		static int mArgc;
		static char** mArgs;

		/**  @brief FTPClient member to be tested in the test suite.
		 **/
		static FTP_client* p_ftp;
	};
}

#endif /* TESTFTPCLIENT_H */