#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <iostream>
#include <stdio.h> 
#include <string.h>
#include <sqlite3.h>


class SQLite3
{  
private:
    static sqlite3 *db;
	static char *zErrMsg;
    static char *sql;
public:
    SQLite3();
    static void SQLinsert();
    static void SQLselect(const char *columns, const char *table, const char *condition);

};

int callback(void *NotUsed, int argc, char **argv, char **azColName);

extern SQLite3 sqlDB;

#endif /* SQLITE3_HPP */
