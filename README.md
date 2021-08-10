# SQLiteBackup
The SQLiteBackup project

## Directory structure

```bash
.
|-- README.md                         - Readme file   
|-- SQLiteBackup                      - SQLiteBackup project directory
|   |-- SQLiteBackup.cpp              - the main function where demonstrated functionality of the SQLite and FTPClient classes 
|   |-- SQLiteBackup.vcxproj          - Visual Studio project file for the SQLiteBackup
|   |-- SQLiteBackup.vcxproj.filters  - 
|   |-- SQLiteBackup.vcxproj.user     -
|   `-- src                           - source files directory
|       |-- FTPClient.cpp             - implemetation of the FTPClient and winSocket classes
|       |-- FTPClient.h               - defination of the FTPClient and winSocket classes
|       |-- SQLite.cpp                - implemetation of the SQLite class
|       |-- SQLite.h                  - defination of the SQLite class
|       `-- sqlite                    - the sqlite3 API, downloaded from https://www.sqlite.org/2021/sqlite-amalgamation-3360000.zip
|           |-- shell.c               -
|           |-- sqlite3.c             -
|           |-- sqlite3.h             - the sqlite3 header file
|           `-- sqlite3ext.h          -
|-- SQLiteBackup.sln                  - Visual Studio solution file for the SQLiteBackup
`-- SQLiteBackupTest                  - SQLiteBackupTest project directory
    |-- SQLiteBackupTest.cpp          - the main function where called the unit test suite for the SQLite and FTPClient classes
    |-- SQLiteBackupTest.vcxproj      - Visual Studio project file for the SQLiteBackupTest
    |-- SQLiteBackupTest.vcxproj.user -
    |-- TestFTPClient.cpp             - implementation of the TestFTPClient class, and the test cases for the FTPClient
    |-- TestFTPClient.h               - defination of the TestFTPClient class for performing unit tests for the FTPClient functionality
    |-- TestSQLite.cpp                - implementation of the TestSQLite class, and the test cases for the SQLite
    |-- TestSQLite.h                  - defination of the TestSQLite class for performing unit tests for the SQLite functionality
    |-- packages.config               - package configuration needed for the test environment, used gtest, version=1.7.0
    `-- sqlite                        - the sqlite3 precompiled binaries, downloaded from https://www.sqlite.org/2021/sqlite-tools-win32-x86-3360000.zip 
        |-- sqlite3.def               - module-definition file
        |-- sqlite3.dll               - dinamic link libruary for the sqlite3 API
        `-- sqlite3.exe               - sqlite3 executable file
```

## Requirements
- Visual Studio 2019
- C++14
- FTP server

## Compiling and usage
1. open the solution with Visual Studio
2. build the solution
3. execute

## Unit tests
Used Google Test freamwork, version 1.7.0.
To enable unit tests:
1. open the solution with Visual Studio
2. right click on the solution, open properties
3. choose "Startup Project"
3. choose "Multiple startup projects"
4. change the state of the SQLiteBackup and SQLiteBackupTest projects to the "start"
5. moove up the SQLiteBackupTest
6. build the project
7. execute

## Default configuration of the FTP server
- ip: 127.0.0.1
- port: 21
- user: admin
- passwd: admin

The values can be changed in the SQLiteBackup.cpp and TestFTPClient.cpp