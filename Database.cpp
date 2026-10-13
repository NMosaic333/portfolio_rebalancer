#include "Database.h"

Database::Database(const std::string& connectionString)
    : connection(connectionString)
{
}

pqxx::connection& Database::getConnection()
{
    return connection;
}