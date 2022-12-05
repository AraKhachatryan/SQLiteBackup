#ifndef  SQLITE_H
#define  SQLITE_H

#include <string>
#include <vector>
#include <map>
#include <winsock2.h>
#include <Windows.h>
#include "sqlite/sqlite3.h"
 
/**   @brief Custom exception class
 */
class SQLiteException: public std::exception
{
public:
    SQLiteException(const std::string& message) throw() { errmsg = message; };
    virtual ~SQLiteException() throw () {};
    virtual const char* what() const throw () { return errmsg.c_str(); } 
protected:
    std::string errmsg;
};
 
/**   @brief structure for representing a query result
 */
typedef struct Query_result {
    std::vector<std::string> column_names;
    std::vector<std::vector<std::string>> result;
    bool colomn_names_exist = false;    
    size_t get_column_count() { return column_names.size(); };
    size_t get_row_count() { return result.size(); };
} Query_result;

/**   @brief Struct for database synchronization during multithreading
 */
typedef struct Sync_db
{
    CRITICAL_SECTION _cs;
    Sync_db() { ::InitializeCriticalSection(&_cs); }
    void lock_db() { ::EnterCriticalSection(&_cs); }
    void unlock_db() { ::LeaveCriticalSection(&_cs); }
    ~Sync_db() { ::DeleteCriticalSection(&_cs); }
} Sync_db;
 
/**
 *    @brief class for handling database connection and executing sql query
 */
class SQLite
{
private:
    /**  @brief SQLITE database connection object
    */
    sqlite3* p_db;

    /**  @brief Database synchronization during multithreading
     **/
    Sync_db* sync;

public:
    /**  Database result set
    */
    static Query_result m_query_result;

    /**  @brief Creates a database connection
     **/
    SQLite();

    /**  @brief Creates database connection to the specified database.
     *   @param db_name database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    SQLite(const std::string& db_name);
 
    /**  Destructor
     **/
    virtual ~SQLite();
 
    /**  @brief To make connection to a database.
     *   @param db_name database name.
     *   @exception SQLiteException thrown if unable to connect to the database.
     **/
    void connect(const std::string& db_name);

    /**  @brief Closes databse connection..
     *   @exception SQLiteException thrown if unable to close the connection.
     **/
    void close();

    /**  @brief Executes SQL commands provided by sql_query argument
     *          Can consist of more than one SQL command.
     *   @param sql_query sql query
     *   @exception SQLiteException thrown if encounters an error while executing the query.
     **/
    int execute(const std::string& sql_query);

    /**  @brief Prints the qurey result
     **/
    void display_query_result();
 
     /** @brief To get the result set.
     **/
    Query_result* get_query_result();
 
    /**  @brief Begin an atomic transaction
     **/ 
    void begin_transaction();

    /**  @brief Commit all atomic transactions
     **/
    void commit_transaction();

    /**  @brief Revert all atomic transactions
     **/
    void rollback();
 
private:
    /**  @brief To clear result array.
     **/
    void clear_records();

    /**  @brief Callback function executed when executing a select query.
     **/
    static int callback(void *data, int argc, char **argv, char **azColName);
};

/**
 *    @brief helper class for exporting database
 */
class DatabaseExporter
{
public:
    explicit DatabaseExporter(SQLite& db)
        : m_db(db) {}

    /**  @brief To dump the database into SQL file
     *   @param sql_file_name file name
     **/
    void dump(const std::string& sql_file_name);

private:
    SQLite& m_db;
};

#endif 