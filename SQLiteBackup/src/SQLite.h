#ifndef  SQLITE_H
#define  SQLITE_H

#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
#include <Windows.h>
#include "sqlite/sqlite3.h"
 
/**
 *   Custom exception class
 */
class SQLiteException: public std::exception
{
public:
    /**
   *   Construct a SQLiteException with a message.
   *   @param message explanatory message
   */
    SQLiteException(const std::string &message) throw();
 
    /** Destructor.
     */
    virtual ~SQLiteException() throw ();
 
    /** Returns a pointer to the (constant) error description.
     */
    virtual const char* what() const throw (){  return errmsg.c_str(); }
 
protected:
    /** Error message.
     */
    std::string errmsg;
};
 
/**
 *    structure for representing a query result
 */
typedef struct queryResult {
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> result;
    bool isColumnNamesSet = false;
    
    size_t	getColumnCount() { return columnNames.size(); };
    size_t	getRowCount() { return result.size(); };

} queryResult;

/**
 *    Struct for Sync database for Multithreading
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
 *    class for handling database connection and executing sql query
 */
class SQLite
{
private:
    /** SQLITE  database connection object
    */
    sqlite3* m_pDb;

     /**
     *   Sync Database in Case of Multiple Threads using class object
     **/
    syncDB* sync;
 
    /** Database result set
    */
    static queryResult m_queryResult;
public:
    /**
     *   Creates a database connection
     **/
    SQLite();
    /**
     *   Creates database  connection to the specified database.
     *   @param sDbName database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    SQLite(const std::string& sDbName);
 
    /**
     *   Destructor
     **/
    virtual ~SQLite();
 
    /**
     *   To make connection to a database.
     *   @param sDbName database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    void connect(const std::string& sDbName);
    /**
     *   Closes databse connection..
     *   @exception SQLiteException thrown if unable to close the connection.
     **/
    void close();
    /**
     *   Executes SQL commands provided by sQuery argument
     *   Can consist of more than one SQL command.
     *   @param sQuery sql query
     *   @exception SQLiteException thrown if encounters an error while executing the query.
     **/
    int execute(const std::string& sQuery);

    /**
     *   Prints the qurey result
     **/
    void printQueryResult();

    /**
     *   To dump the database into SQL file
     *   @exception SQLiteException thrown if encounters an error .
     **/
    void dump(const std::string& sqlFileNam);
 
     /**
     *   To get the result set.
     *   @exception SQLiteException thrown if encounters an error .
     **/
    queryResult* getQueryResult();
 
    /**
     *   Begins an atomic transaction
     *   @exception SQLiteException thrown if encounters an error .
     **/ 
    void beginTransaction();

    /**
     *   Commit all atomic transaction
     *   @exception SQLiteException thrown if encounters an error .
     **/
    void commitTransaction();

    /**
     *   To revert all atomic transaction
     *   @exception SQLiteException thrown if encounters an error .
     **/
    void rollBack();
 
private:
    /**
     *   To clear result array.
     *   @exception std::exception thrown if encounters an error .
     **/
    void clearRecords();
    /**
     *   Callback function executed when executing a select query.
     *   @exception std::exception thrown if encounters an error .
     **/
    static int callback(void *data, int argc, char **argv, char **azColName);
};

#endif 