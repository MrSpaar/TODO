#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <sqlite3.h>


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

template<typename T>
std::string normalize(const T &value) {
    size_t pos = 0;
    std::string normalized = value;

    while ((pos = normalized.find('&', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&amp;");
        pos += 5;
    }

    while ((pos = normalized.find('\'', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&apos;");
        pos += 6;
    }

    pos = 0;
    while ((pos = normalized.find('"', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&quot;");
        pos += 6;
    }

    return normalized;
}
