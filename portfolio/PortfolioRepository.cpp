#include "PortfolioRepository.h"
#include <pqxx/pqxx>
#include<string>
#include<vector>

PortfolioRepository::PortfolioRepository(Database& database):database(database){}

Portfolio PortfolioRepository::loadPortfolio(int portfolioId){
    pqxx::work transaction(database.getConnection());

    pqxx::result result=transaction.exec_params(
        R"(
            SELECT
                p.id AS portfolio_id,
                p.name AS portfolio_name,
                p.cash,

                h.quantity,

                s.id AS security_id,
                s.ticker,
                s.name AS security_name,
                s.sector,

                pr.price

            FROM portfolios p
            JOIN holdings h
                ON p.id = h.portfolio_id

            JOIN securities s
                ON h.security_id = s.id

            JOIN LATERAL (
                SELECT price
                FROM prices
                WHERE security_id = s.id
                ORDER BY timestamp DESC
                LIMIT 1
            ) pr ON true

            WHERE p.id = $1
        )",portfolioId
    );

    if(result.empty()){
        throw std::runtime_error(
            "Portfolio not found or has no holdings."
        );
    }
      
    std::vector<Position> positions;
    for(const auto& row:result){
        Security security(
            row["security_id"].as<int>(),
            row["ticker"].as<std::string>(),
            row["security_name"].as<std::string>(),
            row["sector"].as<std::string>(),
            row["price"].as<double>()
        );

        Position position(
            security,
            row["quantity"].as<double>()
        );
        positions.push_back(position);
    }

    transaction.commit();

    int id=result[0]["portfolio_id"].as<int>();
    std::string name=result[0]["portfolio_name"].as<std::string>();
    double cash=result[0]["cash"].as<double>();

    return Portfolio(
        id,
        name,
        cash,
        positions
    );
}