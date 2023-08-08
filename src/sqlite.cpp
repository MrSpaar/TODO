//
// Created by mrspaar on 8/6/23.
//

#include "sqlite.h"


void sendNotification(const std::string &title, const std::string &body) {
    NotifyNotification *notification = notify_notification_new(title.c_str(), body.c_str(), "dialog-error");

    if (notification == nullptr) {
        std::cerr << "Failed to create notification" << std::endl;
        return;
    }

    notify_notification_set_timeout(notification, 3000);
    notify_notification_show(notification, nullptr);
    g_object_unref(G_OBJECT(notification));
}


void SQLInit(sqlite3 **conn, const char *path) {
    if (int rc = sqlite3_open(path, conn)) {
        sendNotification("Error opening database", sqlite3_errstr(rc));
        exit(1);
    }

    char *errMsg = nullptr;
    sqlite3_exec(*conn, "CREATE TABLE IF NOT EXISTS todo (\n"
                        "    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                        "    description TEXT NOT NULL,\n"
                        "    checked INTEGER NOT NULL DEFAULT 0 CHECK ( checked IN (0, 1) )\n"
                        ");", nullptr, nullptr, &errMsg);

    if (errMsg) {
        sendNotification("Error creating table", errMsg);
        sqlite3_free(errMsg);
        exit(1);
    }
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


void SQLExec(sqlite3 *conn, const std::string &sql, int (*callback)(void*, int, char**, char**), void *res, bool errorOut) {
    char *errMsg = nullptr;
    sqlite3_exec(conn, sql.c_str(), callback, res, &errMsg);

    if (errMsg) {
        sendNotification("Error executing task", '"' + sql + "\" " + errMsg);
        sqlite3_free(errMsg);
    }

    if (errorOut && errMsg)
        exit(1);
}
