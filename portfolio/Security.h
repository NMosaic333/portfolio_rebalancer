#pragma once

#include <string>
#include <vector>

class Security {
private:
    int id;
    std::string ticker;
    std::string name;
    std::string sector;
    double currentPrice;

public:
    Security(
        int id,
        const std::string& ticker,
        const std::string& name,
        const std::string& sector,
        double currentPrice
    );

    int getId() const;
    std::string getName() const;
    std::string getTicker() const;
    double getCurrentPrice() const;
    std::string getSector() const;
    void updatePrice(double newPrice);
};

class Position {
private:
    Security security;
    double quantity;

public:
    Position(
        const Security& security,
        double quantity
    );

    double getValue() const;
    double getQuantity() const;

    Security getSecurity() const;
    int getSecurityId() const;

    void addQuantity(double amount);
    void removeQuantity(double amount);

    void updatePrice(double newPrice);
};

class Portfolio {
private:
    int id;
    std::string name;
    std::vector<Position> positions;
    double cash;

public:
    Portfolio(
        int id,
        const std::string& name,
        double initialCash,
        const std::vector<Position>& positions
    );

    std::vector<Position>getPositions() const;

    void addPosition(const Position& position);
    void removePosition(int securityId);
    void updatePosition(int securityId,double quantity);

    double getTotalValue() const;
    double getPositionValue(int securityId) const;
    double getPortfolioValue() const;

    double getWeight(int securityId) const;
    double getCash() const;
    void updateCash(double cashPrice) ;

    void updatePrice(int securityId, double newPrice);
    double getPrice(int securityId) const;
};