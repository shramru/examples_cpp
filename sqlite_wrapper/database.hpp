//
// Created by vladislav on 30.06.16.
//

#ifndef PROJECT_DATABASE_HPP
#define PROJECT_DATABASE_HPP

#include <sqlite3.h>
#include <string>
#include <functional>
#include <memory>
#include "resultset.hpp"

class DatabaseRuntimeError : public std::runtime_error {
public:
    DatabaseRuntimeError(const std::string &addInfo = "")
            : std::runtime_error("Database error. " + addInfo) {}
};

/**
 * @brief Класс для доступа к базе данных SQLite
 * @description Умеет открывать соединение с базой, выполнять SQL запросы
 */
class Database {
    typedef std::function<void (const ResultSet&)> handlerFunc;

    static bool isFileExists(const std::string& fileName);

    sqlite3* db;
    int pos;

    std::string getErrorMessage();

    inline int set(sqlite3_stmt* stmt, int pos, const std::string& value);
    inline int set(sqlite3_stmt* stmt, int pos, int value);
    inline int set(sqlite3_stmt* stmt, int pos, double value);
    inline int set(sqlite3_stmt* stmt, int pos, std::nullptr_t);

    inline void bind(sqlite3_stmt* stmt);

    template <class Arg>
    inline void bind(sqlite3_stmt* stmt, Arg&& arg);

    template <class Arg, class ...Args>
    inline void bind(sqlite3_stmt* stmt, Arg&& arg, Args&&... args);

public:
    /**
     * @desc Выполняет SQL запрос с заданными парметрами, помещает результат в ResultSet
     * @param query SQL запрос с параметрами
     * @param handler Функция, обрабатывающая ResultSet
     * @param args Параметры. Выглядят как: ?1, ?2, ?3 и т.д. Одна цифра означает один и тот же параметр.
     * @throws DatabaseRuntimeError если не может корректно выполнить запрос
     */
    template <class ...Args>
    void execQuery(const std::string& query, const handlerFunc& handler, Args&&... args);

    /**
     * @desc Выполняет SQL запрос с заданными парметрами
     * @param query SQL запрос с параметрами
     * @param args Параметры. Выглядят как: ?1, ?2, ?3 и т.д. Одна цифра означает один и тот же параметр.
     * @throws DatabaseRuntimeError если не может корректно выполнить запрос
     */
    template <class ...Args>
    void execUpdate(const std::string& query, Args&&... args);

    /**
     * @desc Конструктор Database
     * @param filename Путь к файлу базы данных
     * @param fileMustExist Должен ли обязательно существовать файл БД
     * @throws DatabaseRuntimeError если fileMustExist=true и файл не существует
     */
    Database(const std::string& filename, bool fileMustExist = true);
    ~Database();
};

#include "database.tcc"

typedef std::shared_ptr<Database> DatabasePtr;

#endif //PROJECT_DATABASE_HPP
