//
// Created by mrspaar on 8/6/23.
//

#include <iostream>
#include "sqlite.h"


void SQLInit(sqlite3 **conn, const char *path) {
    int rc = sqlite3_open(path, conn);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(*conn) << std::endl;
        exit(1);
    }

    sqlite3_exec(*conn, "CREATE TABLE IF NOT EXISTS todo (\n"
                        "    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                        "    description TEXT NOT NULL,\n"
                        "    checked INTEGER NOT NULL DEFAULT 0 CHECK ( checked IN (0, 1) )\n"
                        ");", nullptr, nullptr, nullptr);
}


int SQLFetchAll(void* res, int argc, char **argv, char **colNames) {
    if (argc == 0)
        return 1;

    auto *data = (SQLData*) res;

    SQLRow row;
    for (int i = 0; i < argc; i++)
        row.emplace(colNames[i], argv[i] ? argv[i] : "NULL");

    data->push_back(row);
    return 0;
}


int SQLFetchOne(void* res, int argc, char **argv, char **colNames) {
    if (argc == 0)
        return 1;

    auto *row = (SQLRow*) res;
    for (int i = 0; i < argc; i++)
        row->emplace(colNames[i], argv[i] ? argv[i] : "NULL");

    return 0;
}
