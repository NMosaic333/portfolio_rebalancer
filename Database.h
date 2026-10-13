#pragma once

#include <pqxx/pqxx>
#include <string>

class Database
{
private:
    pqxx::connection connection;

public:
    Database(const std::string& connectionString);

    pqxx::connection& getConnection();
};