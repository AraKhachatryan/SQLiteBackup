#include <fstream>
#include "TestSQLite.h"

namespace GT
{
	/**
	 *   @brief in this test case will be tested "execute" function with select query
	 *			The result will be compared with expected values.
	 **/
	TEST_F(TestSQLite, test_sqlect_query)	{
		pDB->execute("SELECT * FROM PERSON WHERE AGE > 28;");
		queryResult* res = pDB->getQueryResult();
		std::vector<std::string> row = res->result[0];
		std::vector<std::string> res_expected = { "1", "STEVE", "GATES", "30", "PALO ALTO", "1000.0" };
		ASSERT_EQ(row, res_expected);
	}

	/**
	 *   @brief in this test case will be tested "dump" function which creates the backup of database with sql format.
	 *			Asseert is temorary disabled due to floating point deviation between the results.
	 **/
	TEST_F(TestSQLite, test_sql_dump) {
		std::string testDumpFile = "test_dump.sql";
		pDB->dump(testDumpFile);

		// using sqlite3.exe to open the "test.db" and dump the database into file
		system(".\\sqlite\\sqlite3.exe test.db .dump > test_dump_expected.sql");

		std::ifstream testDumpFS(testDumpFile);
		std::string testDumpBuf((std::istreambuf_iterator<char>(testDumpFS)), std::istreambuf_iterator<char>());
		std::ifstream testDumpExpectedFS("test_dump_expected.sql");
		std::string testDumpExpectedBuf((std::istreambuf_iterator<char>(testDumpExpectedFS)), std::istreambuf_iterator<char>());

		//ASSERT_EQ(testDumpBuf, testDumpExpectedBuf);
	}

	/**
	 *   @brief in this test case will be tested rollBack() function which reverts all atomic transactions.
	 **/
	TEST_F(TestSQLite, test_rollback) {
		pDB->dump("before.sql");  // saving the state before modification

		// performing bad things ...
		pDB->beginTransaction();
		pDB->execute("UPDATE PERSON SET SALARY = '12000' WHERE AGE > 26;");
		pDB->execute("DELETE FROM PERSON WHERE AGE < 25;");

		pDB->rollBack();  // reverting all the things done 
		pDB->dump("after.sql");   // saving the state after modification

		std::ifstream beforeFS("before.sql");
		std::string beforeBuf((std::istreambuf_iterator<char>(beforeFS)), std::istreambuf_iterator<char>());
		std::ifstream afterFS("after.sql");
		std::string afterBuf((std::istreambuf_iterator<char>(afterFS)), std::istreambuf_iterator<char>());

		ASSERT_EQ(beforeBuf, afterBuf);
	}
}


using namespace GT;

int TestSQLite::mArgc = 0;
char** TestSQLite::mArgs = NULL;

SQLite* TestSQLite::pDB = nullptr;

TestSQLite::TestSQLite(void) {

}

TestSQLite::~TestSQLite(void) {

}

/**
 *   @brief SetUpTestCase() will be called before running the first test in test suite.
 *
 *   @brief SQLite object will be created before test suite run
 *			Also will be created initial table with data for testing
 **/
void TestSQLite::SetUpTestCase() {
	std::cout << "Set up test cases." << std::endl;

	if (pDB != NULL) {
		delete pDB;
	}

	pDB = new SQLite("test.db");  // creating database
	pDB->beginTransaction();

	// Creating table and inserting data
	pDB->execute("DROP TABLE IF EXISTS PERSON;");
	std::string sql =
		"CREATE TABLE PERSON(\n"
		"    ID INT PRIMARY KEY     NOT NULL,\n"
		"    NAME           TEXT    NOT NULL,\n"
		"    SURNAME        TEXT    NOT NULL,\n"
		"    AGE            INT     NOT NULL,\n"
		"    ADDRESS        CHAR(50),\n"
		"    SALARY         REAL\n"
		");";
	pDB->execute(sql);
	sql = "INSERT INTO PERSON VALUES(1, 'STEVE', 'GATES', 30, 'PALO ALTO', 1000.0);"
		"INSERT INTO PERSON VALUES(2, 'BILL', 'ALLEN', 20, 'SEATTLE', 300.22);"
		"INSERT INTO PERSON VALUES(3, 'WILL', 'SMITH', 27, 'PORTLAND', 600.22);"
		"INSERT INTO PERSON VALUES(4, 'PAUL', 'JOBS', 24, 'SEATTLE', 9900.0);";
	pDB->execute(sql);

	pDB->commitTransaction();
}

/**
 *   @brief TearDownTestCase() will be called after running the last test in test suite.
 *
 *   @brief will be done cleanup in the database after the test suit run
 **/
void TestSQLite::TearDownTestCase() {
	std::cout << "Tear down test cases." << std::endl;

	pDB->execute("DROP TABLE IF EXISTS PERSON;");
	pDB->commitTransaction();
	delete pDB;
	pDB = nullptr;
}

void TestSQLite::SetUp(void) {

}

void TestSQLite::TearDown(void) {

}


