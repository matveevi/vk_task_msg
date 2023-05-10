#include <iostream>
#include "sqlite3.h"
#include "db_tools.h"

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

static int insert_into_db(Person new_person) {
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

    // Создаем таблицу если еще не создали
    sql = "CREATE TABLE IF NOT EXISTS PERSON("  \
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

    // Вставляем данные в таблицу
    sql = "INSERT INTO PERSON (ID, NAME, PASS, LOGIN) "  \
         "VALUES (" + std::to_string(new_person.id) + ", '" +
         new_person.name + "', '" + new_person.login + "', '" +
         new_person.password + "' );";

    rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }

    // Закрываем базу данных
    sqlite3_close(db);
}

static bool check_user_in_db(Person searched_person) {
    sqlite3 *db;

    // Открываем базу данных
    int rc = sqlite3_open("test.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    sqlite3_stmt *stmt;
    // std::string sql = sql = "SELECT COUNT(*) FROM PERSON WHERE NAME = '"  \
    //     + searched_person.name + "' AND LOGIN = '" +
    //      searched_person.login + "' AND PASS = '" + 
    //      searched_person.password + "'";
    std::string sql = "SELECT COUNT(*) FROM PERSON WHERE NAME = ? AND LOGIN = ? AND PASS = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, nullptr);
    if(rc != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db);
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, std::string("name").c_str(), -1, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_bind_text(stmt, 2, searched_person.login.c_str(), -1, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_bind_text(stmt, 3, searched_person.password.c_str(), -1, SQLITE_TRANSIENT);
    if(rc != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count > 0;
    } else {
        sqlite3_finalize(stmt);
        return false;
    }
}
