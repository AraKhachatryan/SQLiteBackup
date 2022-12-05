#include <iostream>
#include "src/SQLite.h"
#include "src/FTPClient.h"


int main()
{
    SQLite* db = new SQLite("example.db");
    int total_changes = 0;

    db->begin_transaction();

    db->execute("DROP TABLE IF EXISTS PERSON;");
    std::string sql =
        "CREATE TABLE PERSON(\n"
        "    ID INT PRIMARY KEY     NOT NULL,\n"
        "    NAME           TEXT    NOT NULL,\n"
        "    SURNAME        TEXT    NOT NULL,\n"
        "    AGE            INT     NOT NULL,\n"
        "    ADDRESS        CHAR(50),\n"
        "    SALARY         REAL\n"
        ");";
    db->execute(sql);

    sql = "INSERT INTO PERSON VALUES(1, 'STEVE', 'GATES', 30, 'PALO ALTO', 1000.0);"
        "INSERT INTO PERSON VALUES(2, 'BILL', 'ALLEN', 20, 'SEATTLE', 300.22);"
        "INSERT INTO PERSON VALUES(3, 'WILL', 'SMITH', 27, 'PORTLAND', 600.22);"
        "INSERT INTO PERSON VALUES(4, 'PAUL', 'JOBS', 24, 'SEATTLE', 9900.0);";
    for (int i = 5; i < 50; ++i) {
        int age = (rand() % 50) + 20;
        sql += "INSERT INTO PERSON VALUES(" + std::to_string(i) + ", 'BILL', 'ALLEN', " + std::to_string(age) + ", 'SEATTLE', 300.22);";
    }
    total_changes = db->execute(sql);
    std::cout << "total changes: " << total_changes << std::endl;

    db->execute("SELECT * FROM PERSON WHERE AGE < 30;");
    db->display_query_result();

    db->execute("DROP TABLE IF EXISTS coffees;");
    sql =
        "CREATE TABLE coffees (\n"
        "    id INTEGER PRIMARY KEY,\n"
        "    coffee_name TEXT NOT NULL,\n"
        "    price REAL NOT NULL\n"
        ");\n";
    db->execute(sql);

    sql = "INSERT INTO coffees VALUES(1,'Colombian',7.99);"
        "INSERT INTO coffees VALUES(2, 'French_Roast', 8.99);"
        "INSERT INTO coffees VALUES(3, 'Espresso', 9.99);"
        "INSERT INTO coffees VALUES(4, 'Colombian_Decaf', 8.99);"
        "INSERT INTO coffees VALUES(5, 'French_Roast_Decaf', 9.99);";
    for (int i = 6; i < 50; ++i) {
        double price = (rand() % 7) + 2.99;
        sql += "INSERT INTO coffees VALUES(" + std::to_string(i) + ", 'French_Roast_Decaf', " + std::to_string(price) + ");";
    }
    total_changes = db->execute(sql);
    std::cout << "total changes: " << total_changes << std::endl;

    db->commit_transaction();

    std::string sql_dump_file = "export.sql";
    DatabaseExporter exporter(*db);
    exporter.dump(sql_dump_file);

    delete db;
    std::cout << std::endl;

    std::string ip = "127.0.0.1";
    std::string port = "21";
    std::string user = "admin";
    std::string passwd = "admin";
    std::string dir = "sql";

    try {
        FTP_client* ftp = new FTP_client(ip, port, user, passwd);

        // see FTP commands https://en.wikipedia.org/wiki/List_of_FTP_commands
        ftp->execute_command("PWD");

        // Command that requires data channel
        ftp->request_new_data_channel();
        ftp->execute_command("LIST");
        std::string ls_response = ftp->read_data(); // read from data channel

        // assumming that the user have all permissions on the ftp directory
        // make directory in the FTP server
        std::string mkdir = "MKD " + dir;
        ftp->execute_command(mkdir);

        // Command that requires data channel
        ftp->request_new_data_channel();
        ftp->execute_command("LIST");
        ftp->read_data(); // read from data channel

        // Backup the database file into FTP server
        std::ifstream fs(sql_dump_file);
        std::string sql_dump_buf((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
        std::string send_file_cmd = "STOR " + dir + "/" + sql_dump_file;
        ftp->request_new_data_channel();
        ftp->execute_command(send_file_cmd);
        ftp->send_data(sql_dump_buf);

        delete ftp;
    }
    catch (const std::runtime_error& error) {
        std::cout << "Error: communication via FTP failed: " << error.what() << std::endl;
    }

    std::cout << "Execution finished succesfully." << std::endl;
    std::cin.get();
    return 0;
}