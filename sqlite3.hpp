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
    static void insertAnchor(const char *id, const char * x, const char * y, const char * timestamp);
    static void insertTag(const char *id, const char * timestamp);
    static void addRange(const char *tag_id, const char *anchor_id, const char *distance, const char *timestamp);

};

int callback(void *NotUsed, int argc, char **argv, char **azColName);

extern SQLite3 sqlDB;

#endif /* SQLITE3_HPP */
