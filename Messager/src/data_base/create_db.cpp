#include <iostream>
#include "sqlite3.h"
#include "person.h"

// Функция обратного вызова для выполнения SQL-запросов
static int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string sql;
    const char* data = "Callback function called";

    // Открываем базу данных
    rc = sqlite3_open("test.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    // Создаем таблицу
    sql = "CREATE TABLE PERSON("  \
         "ID INT PRIMARY KEY      NOT NULL," \
         "NAME           TEXT     NOT NULL," \
         "PASS           TEXT     NOT NULL,"\
         "LOGIN          TEXT     NOT NULL );";

    rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }

    // Закрываем базу данных
    sqlite3_close(db);

    return 0;
}
