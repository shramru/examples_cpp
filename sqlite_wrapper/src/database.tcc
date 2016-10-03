//
// Created by vladislav on 30.06.16.
//

#ifndef PROJECT_DATABASE_HPP
#error Include database.hpp instead of this
#endif

inline int Database::set(sqlite3_stmt* stmt, int pos, const std::string& value) {
    return sqlite3_bind_text(stmt, pos, value.c_str(), -1, SQLITE_STATIC);
}

inline int Database::set(sqlite3_stmt* stmt, int pos, int value) {
    return sqlite3_bind_int(stmt, pos, value);
}

inline int Database::set(sqlite3_stmt* stmt, int pos, double value) {
    return sqlite3_bind_double(stmt, pos, value);
}

inline int Database::set(sqlite3_stmt* stmt, int pos, std::nullptr_t /* value */) {
    return sqlite3_bind_null(stmt, pos);
}

inline void Database::bind(sqlite3_stmt* stmt) {}

template <class Arg>
inline void Database::bind(sqlite3_stmt* stmt, Arg&& arg) {
    if (set(stmt, ++pos, arg) != SQLITE_OK) {
        throw DatabaseRuntimeError("Can't bind parameter to SQL Query. Message: " + getErrorMessage());
    }
}

template <class Arg, class ...Args>
inline void Database::bind(sqlite3_stmt* stmt, Arg&& arg, Args&&... args) {
    bind(stmt, std::forward<Arg>(arg));
    bind(stmt, std::forward<Args>(args)...);
}

template <class ...Args>
void Database::execQuery(const std::string& query, const handlerFunc& handler, Args&&... args) {
    pos = 0;

    int rc;
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        throw DatabaseRuntimeError("Can't prepare SQL Query. Message: " + getErrorMessage());
    }

    bind(stmt, std::forward<Args>(args)...);
    int columns = sqlite3_column_count(stmt);

    ResultSet resultSet;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        ResultSet::row_type row;
        for (int i = 0; i < columns; ++i) {
            std::string column_name = sqlite3_column_name(stmt, i);
            std::string value;
            switch (sqlite3_column_type(stmt, i)) {
                case SQLITE_INTEGER:
                    value = std::to_string(sqlite3_column_int(stmt, i));
                    break;
                case SQLITE_FLOAT:
                    value = std::to_string(sqlite3_column_double(stmt, i));
                    break;
                case SQLITE_TEXT:
                    value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                    break;
                case SQLITE_NULL:
                    value = "{null}";
                    break;
            }
            row.emplace_back(column_name, value);
        }
        resultSet.addRow(row);
    }

    if (rc != SQLITE_DONE) {
        throw DatabaseRuntimeError("Can't step SQL Query. Message: " + getErrorMessage());
    }

    sqlite3_finalize(stmt);

    if (handler) handler(resultSet);
}

template <class ...Args>
void Database::execUpdate(const std::string& query, Args&&... args) {
    execQuery(query, nullptr, std::forward<Args>(args)...);
}