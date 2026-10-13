#pragma once

#include "Security.h"
#include "../Database.h"

class PortfolioRepository
{
private:
    Database& database;

public:
    PortfolioRepository(Database& database);

    Portfolio loadPortfolio(int portfolioId);
};