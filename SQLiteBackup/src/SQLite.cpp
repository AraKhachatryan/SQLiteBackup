#include <iostream>
#include "SQLite.h"
#include <fstream>
#include <iomanip>

SQLite::SQLite() {
    sync = new Sync_db();
}

SQLite::SQLite(const std::string& db_name) {
    sync = new Sync_db();
    int rc = sqlite3_open(db_name.c_str(), &p_db);
    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(p_db));
        throw SQLiteException("Can't open database:  " + errmsg);
    } else {
        std::cout << "Opened " << db_name << " database successfully" << std::endl;
    }
}

SQLite::~SQLite() {
    delete sync;
    if (p_db != NULL) {
        close();
    }
    clear_records();
}

void SQLite::connect(const std::string& db_name) {
    int rc = sqlite3_open(db_name.c_str(), &p_db);
    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(p_db));
        throw SQLiteException("Can't open database:  " + errmsg);
    } else {
        std::cout << "Opened " << db_name << " database successfully" << std::endl;
    }
}

void SQLite::close() {
    if (p_db != NULL) {
        int rc = sqlite3_close(p_db);
        if (rc != SQLITE_OK) {
            std::string errmsg(sqlite3_errmsg(p_db));
            throw SQLiteException("Can't close database:  " + errmsg);
        } else {
            std::cout << "Closed the database successfully" << std::endl;
        }
    }
}

int SQLite::execute(const std::string& sql_query) {
    /* Clear the records first*/
    clear_records();
    /* Execute SQL statement */
    sync->lock_db();
    int rc = sqlite3_exec(p_db, sql_query.c_str(), &SQLite::callback, NULL, NULL);
    sync->unlock_db();
    if (rc != SQLITE_OK) {
        std::string errmsg(sqlite3_errmsg(p_db));
        std::cout << "SQLite error: " << errmsg << std::endl;
        throw SQLiteException("SQL error:  " + errmsg);
    }
    return sqlite3_total_changes(p_db);
}

void SQLite::clear_records() {
    m_query_result.colomn_names_exist = false;
    m_query_result.column_names.clear();
    m_query_result.result.clear();
}

int SQLite::callback(void* data, int argc, char** argv, char** azColName) {
    m_query_result.result.push_back({});
    for (int i = 0; i < argc; i++) {
        if (!m_query_result.colomn_names_exist) {
            m_query_result.column_names.emplace_back(azColName[i] ? azColName[i] : "");
        }
        m_query_result.result.back().emplace_back(argv[i] ? argv[i] : "");
    }
    m_query_result.colomn_names_exist = true;
    return 0;
}

void SQLite::display_query_result() {
    for (auto column: m_query_result.column_names) {
        std::cout << "\033[1;32m" << std::left << std::setw(10) << column << "\033[0m";
    }
    std::cout << std::endl;
    for (auto row: m_query_result.result) {
        for (auto value: row) {
            std::cout << std::left << std::setw(10) << value;
        }
        std::cout << std::endl;
    }
}

void SQLite::begin_transaction() {
    sqlite3_exec(p_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
}

void SQLite::commit_transaction() {
    sqlite3_exec(p_db, "COMMIT;", NULL, NULL, NULL);
}

void SQLite::rollback() {
    sqlite3_exec(p_db, "ROLLBACK;", NULL, NULL, NULL);
}

Query_result* SQLite::get_query_result() {
    return &m_query_result;
}

Query_result SQLite::m_query_result;

void DatabaseExporter::dump(const std::string& sql_file_name) {
    std::ofstream sql_dump;
    sql_dump.open(sql_file_name, std::ios::out);
    sql_dump << "PRAGMA foreign_keys=OFF;\n";
    sql_dump << "BEGIN TRANSACTION;\n";
    m_db.execute("SELECT sql,tbl_name,type FROM sqlite_master WHERE type = 'table';");
    std::vector<std::vector<std::string>> tables = m_db.m_query_result.result;
    for (std::vector<std::string> table : tables) {
        sql_dump << table[0] + ";\n";
        m_db.execute("SELECT * from " + table[1] + ";");
        for (std::vector<std::string> row : m_db.m_query_result.result) {
            sql_dump << "INSERT INTO " + table[1] + " VALUES(";
            bool once = false;
            for (std::string value : row) {
                if (once) {
                    sql_dump << ",";
                }
                once = true;
                if (!value.empty()) {
                    sql_dump << "'" + value + "'";
                }
                else {
                    sql_dump << "NULL";
                }
            }
            sql_dump << ");\n";
        }
    }
    m_db.execute("SELECT sql FROM sqlite_master WHERE type = 'trigger';");
    for (std::vector<std::string> table : m_db.m_query_result.result) {
        sql_dump << table[0] + ";\n";
    }
    sql_dump << "COMMIT;\n";
    sql_dump.close();
    std::cout << "Successfully exported the database into " << sql_file_name << std::endl;
}