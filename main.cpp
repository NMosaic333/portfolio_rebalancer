#include <iostream>
#include "Database.h"
#include "PortfolioRepository.h"
#include "Security.h"

int main()
{
    try{
        Database db(
            "host=localhost "
            "port=5432 "
            "dbname=portfolio_db "
            "user=postgres "
        );

        PortfolioRepository repository(db);

        Portfolio portfolio=repository.loadPortfolio(1);

        std::cout
            << "Portfolio value: "
            << portfolio.getPortfolioValue()
            << std::endl;
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}