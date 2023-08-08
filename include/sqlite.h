//
// Created by mrspaar on 8/6/23.
//

#ifndef TODO_SQLITE_H
#define TODO_SQLITE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <sqlite3.h>
#include <libnotify/notify.h>


typedef std::map<std::string, std::string> SQLRow;
typedef std::vector<SQLRow> SQLData;


void SQLInit(sqlite3 **conn, const char *path);
int SQLFetchAll(void* res, int argc, char **argv, char **colNames);
int SQLFetchOne(void* res, int argc, char **argv, char **colNames);
void SQLExec(sqlite3 *conn, const std::string &sql, int (*callback)(void*, int, char**, char**), void *res, bool errorOut = false);

template<typename T>
T SQLGet(const SQLRow &row, const std::string &colName) {
    if (row.find(colName) == row.end())
        throw std::runtime_error("Column not found: " + colName);

    try {
        if constexpr (std::is_same_v<T, int>)
            return std::stoi(row.at(colName));
        else if constexpr (std::is_same_v<T, bool>)
            return std::stoi(row.at(colName)) == 1;
        else
            return row.at(colName);
    } catch (std::invalid_argument &e) {
        throw std::runtime_error("Invalid type for argument: " + colName);
    } catch (std::out_of_range &e) {
        throw std::runtime_error("Value is too big: " + row.at(colName));
    }
}


#endif //TODO_SQLITE_H
