#include <iostream>
#include "SQLite.h"
#include <fstream>
#include <iomanip>

SQLiteException::SQLiteException(const std::string& message) throw() {
    errmsg = message;
}

SQLiteException::~SQLiteException() throw () {

}

SQLite::SQLite() {
    sync = new syncDB();
}

SQLite::SQLite(const std::string& sDbName) {
    sync = new syncDB();
    int rc = sqlite3_open(sDbName.c_str(), &m_pDb);

    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(m_pDb));
        throw SQLiteException("Can't open database:  " + errmsg);
    } else {
        std::cout << "Opened " << sDbName << " database successfully" << std::endl;
    }
}

SQLite::~SQLite() {
    delete sync;
    if (m_pDb != NULL) {
        close();
    }
    clearRecords();
}

void SQLite::connect(const std::string& sDbName) {
    int rc = sqlite3_open(sDbName.c_str(), &m_pDb);

    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(m_pDb));
        throw SQLiteException("Can't open database:  " + errmsg);
    } else {
        std::cout << "Opened " << sDbName << " database successfully" << std::endl;
    }
}

void SQLite::close() {
    if (m_pDb != NULL) {
        int rc = sqlite3_close(m_pDb);
        if (rc != SQLITE_OK) {
            std::string errmsg(sqlite3_errmsg(m_pDb));
            throw SQLiteException("Can't close database:  " + errmsg);
        } else {
            std::cout << "Closed the database successfully" << std::endl;
        }
    }
}

int SQLite::execute(const std::string& sQuery) {
    /* Clear the records first*/
    clearRecords();
        
    /* Execute SQL statement */
    sync->lockDB();
    int rc = sqlite3_exec(m_pDb, sQuery.c_str(), &SQLite::callback, NULL, NULL);
    sync->unlockDB();
    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(m_pDb));
        std::cout << "SQLite error: " << errmsg << std::endl;
        throw SQLiteException("SQL error:  " + errmsg);
    }
    return sqlite3_total_changes(m_pDb);
}

void SQLite::dump(const std::string& sqlFileName) {
    std::ofstream sqlDump;
    sqlDump.open(sqlFileName, std::ios::out);

    sqlDump << "PRAGMA foreign_keys=OFF;\n";
    sqlDump << "BEGIN TRANSACTION;\n";

    execute("SELECT sql,tbl_name,type FROM sqlite_master WHERE type = 'table';");
    std::vector<std::vector<std::string>> tables = m_queryResult.result;
    for (std::vector<std::string> table: tables) {

        sqlDump << table[0] + ";\n";

        execute("SELECT * from " + table[1] + ";");
        for (std::vector<std::string> row: m_queryResult.result) {
            sqlDump << "INSERT INTO " + table[1] + " VALUES(";
            bool once = false;
            for (std::string value: row) {
                if (once) {
                    sqlDump << ",";
                }
                once = true;
                if (!value.empty()) {
                    sqlDump << "'" + value + "'";
                } else {
                    sqlDump << "NULL";
                }
            }
            sqlDump << ");\n";
        }
    }

    execute("SELECT sql FROM sqlite_master WHERE type = 'trigger';");
    for (std::vector<std::string> table : m_queryResult.result) {
        sqlDump << table[0] + ";\n";
    }

    sqlDump << "COMMIT;\n";
    sqlDump.close();

    std::cout << "Successfully exported the database into " << sqlFileName << std::endl;
}

void SQLite::clearRecords() {
    m_queryResult.isColumnNamesSet = false;
    m_queryResult.columnNames.clear();
    m_queryResult.result.clear();
}

int SQLite::callback(void* data, int argc, char** argv, char** azColName) {
    m_queryResult.result.push_back({});

    for (int i = 0; i < argc; i++) {
        if (!m_queryResult.isColumnNamesSet) {
            m_queryResult.columnNames.emplace_back(azColName[i] ? azColName[i] : "");
        }
        m_queryResult.result.back().emplace_back(argv[i] ? argv[i] : "");
    }
    m_queryResult.isColumnNamesSet = true;

    return 0;
}

void SQLite::printQueryResult() {
    for (auto column: m_queryResult.columnNames) {
        std::cout << "\033[1;32m" << std::left << std::setw(10) << column << "\033[0m";
    }
    std::cout << std::endl;

    for (auto row: m_queryResult.result) {
        for (auto value: row) {
            std::cout << std::left << std::setw(10) << value;
        }
        std::cout << std::endl;
    }
}

void SQLite::beginTransaction() {
    sqlite3_exec(m_pDb, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void SQLite::commitTransaction() {
    sqlite3_exec(m_pDb, "COMMIT;", NULL, NULL, NULL);
}

void SQLite::rollBack() {
    sqlite3_exec(m_pDb, "ROLLBACK;", NULL, NULL, NULL);
}

queryResult* SQLite::getQueryResult() {
    return &m_queryResult;
}

queryResult SQLite::m_queryResult;