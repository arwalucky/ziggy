#include "sqlite3.hpp"

SQLite3 sqlDB;
sqlite3 *SQLite3::db;
char *SQLite3::zErrMsg = 0;
char *SQLite3::sql;

SQLite3::SQLite3()
{
    int rc;
    rc = sqlite3_open("test.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

}

void SQLite3::SQLinsert()
{
    
}

void SQLite3::SQLselect(const char *columns, const char *table, const char *condition)
{
    char prompt[100];

    strcpy(prompt, "SELECT ");
    strcat(prompt, columns);
    strcat(prompt, " FROM ");
    strcat(prompt, table);
    // strcat(prompt, " WHERE ");
    // strcat(prompt, condition);
    strcat(prompt, ";");

    sql = prompt;
    std::cout << sql << std::endl;  
    sqlite3_exec(db, sql, callback, 0, &zErrMsg);
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

