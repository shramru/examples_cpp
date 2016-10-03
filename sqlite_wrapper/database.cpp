//
// Created by vladislav on 30.06.16.
//

#include "database.hpp"
#include <fstream>

Database::Database(const std::string& filename, bool fileMustExist) {
    if (fileMustExist && !isFileExists(filename)) {
        throw DatabaseRuntimeError("Can't connect to database. File not exists.");
    }

    int rc;
    if ((rc = sqlite3_open(filename.c_str(), &db)) != SQLITE_OK) {
        sqlite3_close(db);
        throw DatabaseRuntimeError("Can't connect to database. Code: " + std::to_string(rc));
    }
}

Database::~Database() {
    sqlite3_close(db);
}

bool Database::isFileExists(const std::string& fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

std::string Database::getErrorMessage() {
    return sqlite3_errmsg(db);
}



