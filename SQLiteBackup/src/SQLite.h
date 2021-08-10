#ifndef  SQLITE_H
#define  SQLITE_H

#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
#include <Windows.h>
#include "sqlite/sqlite3.h"
 
/**
 *   @brief Custom exception class
 */
class SQLiteException: public std::exception
{
public:
    /**
   *   @brief Construct a SQLiteException with a message.
   *   @param message explanatory message
   */
    SQLiteException(const std::string &message) throw();
 
    /** @brief Destructor.
     */
    virtual ~SQLiteException() throw ();
 
    /** @brief Returns a pointer to the (constant) error description.
     */
    virtual const char* what() const throw (){  return errmsg.c_str(); }
 
protected:
    /** @brief Error message.
     */
    std::string errmsg;
};
 
/**
 *    @brief structure for representing a query result
 */
typedef struct queryResult {
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> result;
    bool isColumnNamesSet = false;
    
    size_t	getColumnCount() { return columnNames.size(); };
    size_t	getRowCount() { return result.size(); };

} queryResult;

/**
 *    @brief Struct for Sync database for Multithreading
 */
typedef struct sync
{
    CRITICAL_SECTION _cs;
    sync() { ::InitializeCriticalSection(&_cs); }
    void lockDB() { ::EnterCriticalSection(&_cs); }
    void unlockDB() { ::LeaveCriticalSection(&_cs); }
    ~sync() { ::DeleteCriticalSection(&_cs); }
}syncDB;
 
/**
 *    @brief class for handling database connection and executing sql query
 */
class SQLite
{
private:
    /** @brief SQLITE  database connection object
    */
    sqlite3* m_pDb;

    /**
     *   @brief Sync Database in Case of Multiple Threads using class object
     **/
    syncDB* sync;
 
    /** Database result set
    */
    static queryResult m_queryResult;
public:
    /**
     *   @brief Creates a database connection
     **/
    SQLite();

    /**
     *   @brief Creates database  connection to the specified database.
     *   @param sDbName database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    SQLite(const std::string& sDbName);
 
    /**
     *   Destructor
     **/
    virtual ~SQLite();
 
    /**
     *   @brief To make connection to a database.
     *   @param sDbName database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    void connect(const std::string& sDbName);

    /**
     *   @brief Closes databse connection..
     *   @exception SQLiteException thrown if unable to close the connection.
     **/
    void close();

    /**
     *   @brief Executes SQL commands provided by sQuery argument
     *          Can consist of more than one SQL command.
     *   @param sQuery sql query
     *   @exception SQLiteException thrown if encounters an error while executing the query.
     **/
    int execute(const std::string& sQuery);

    /**
     *   @brief Prints the qurey result
     **/
    void printQueryResult();

    /**
     *   @brief To dump the database into SQL file
     *   @param sqlFileNam file name
     **/
    void dump(const std::string& sqlFileNam);
 
     /**
     *   @brief To get the result set.
     **/
    queryResult* getQueryResult();
 
    /**
     *   @brief Begins an atomic transaction
     **/ 
    void beginTransaction();

    /**
     *   @brief Commit all atomic transaction
     **/
    void commitTransaction();

    /**
     *   @brief To revert all atomic transaction
     **/
    void rollBack();
 
private:
    /**
     *   @brief To clear result array.
     **/
    void clearRecords();

    /**
     *   @brief Callback function executed when executing a select query.
     **/
    static int callback(void *data, int argc, char **argv, char **azColName);
};

#endif 