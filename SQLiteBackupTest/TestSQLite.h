#ifndef TESTSQLITE_H
#define TESTSQLITE_H

#include "gtest/gtest.h"
#include "../SQLiteBackup/src/SQLite.h"

namespace GT
{
	//class SQLite;

	class TestSQLite : public ::testing::Test
	{
	public:
		/**
		 *   @brief through constructor can be done set-up work for each test.
		 **/
		TestSQLite(void);

		/**
		 *   @brief through destructor can be done clean-up work for each test.
		 **/
		virtual ~TestSQLite(void) override;

		/**
		 *   @brief SetUpTestCase() will be called before running the first test in test suite.
		 **/
		static void SetUpTestCase();

		/**
		 *   @brief TearDownTestCase() will be called after running the last test in test suite.
		 **/
		static void TearDownTestCase();

		/**
		 *   @brief SetUp() will be called immediately after the constructor (right before each test).
		 **/
		virtual void SetUp(void) override;

		/**
		 *   @brief TearDown() will be called immediately after each test (right before the destructor).
		 **/
		virtual void TearDown(void) override;

	public:
		static int mArgc;
		static char** mArgs;

		/**
		 *   @brief SQLite member to be tested in the test suite.
		 **/
		static SQLite* pDB;
	};
}

#endif /* TESTSQLITE_H */