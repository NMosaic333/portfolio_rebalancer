#include "Security.h"

Security::Security(
    int id,
    const std::string& ticker,
    const std::string& name,
    const std::string& sector,
    double currentPrice
)
    : id(id),
      ticker(ticker),
      name(name),
      sector(sector),
      currentPrice(currentPrice)
{

}

int Security::getId() const {
    return id;
}

std::string Security::getName() const{
    return name;
}

std::string Security::getTicker() const {
    return ticker;
}
std::string Security::getSector() const {
    return sector;
}

double Security::getCurrentPrice() const {
    return currentPrice;
}


void Security::updatePrice(double newPrice) {
    currentPrice = newPrice;
}

Position::Position(
    const Security& security,
    double quantity
)
    : security(security),
      quantity(quantity)
{
}

double Position::getValue() const {
    return quantity * security.getCurrentPrice();
}


double Position::getQuantity() const {
    return quantity;
}


int Position::getSecurityId() const {
    return security.getId();
}
Security Position::getSecurity() const{
    return security;
}

void Position::addQuantity(double amount) {
    quantity += amount;
}


void Position::removeQuantity(double amount) {
    quantity -= amount;
    if (quantity < 0) {
        quantity = 0;
    }
}

void Position::updatePrice(double newPrice) {
    security.updatePrice(newPrice);
}

Portfolio::Portfolio(
    int id,
    const std::string& name,
    double initialCash,
    const std::vector<Position>& positions
)
    : id(id),
      name(name),
      positions(positions),
      cash(initialCash)
{
  
}

std::vector<Position> Portfolio::getPositions() const{
    return positions;
}

void Portfolio::addPosition(const Position& position) {
    positions.push_back(position);
}


void Portfolio::removePosition(int securityId) {
    for (auto it = positions.begin(); it != positions.end(); ++it) {

        if (it->getSecurityId() == securityId) {
            positions.erase(it);
            return;
        }
    }
}

double Portfolio::getTotalValue() const {
    double total = 0.0;

    for (const Position& position : positions) {
        total += position.getValue();
    }

    return total;
}


double Portfolio::getPositionValue(int securityId) const {
    for (const Position& position : positions) {

        if (position.getSecurityId() == securityId) {
            return position.getValue();
        }
    }

    return 0.0;
}


double Portfolio::getPortfolioValue() const {
    return getTotalValue() + cash;
}


double Portfolio::getWeight(int securityId) const {

    double totalValue = getPortfolioValue();

    if (totalValue <= 0.0) {
        return 0.0;
    }

    double positionValue = getPositionValue(securityId);

    return (positionValue / totalValue);
}
double Portfolio::getCash() const{
    return cash;
}

void Portfolio::updatePrice(
    int securityId,
    double newPrice
) {
    for (Position& position : positions) {

        if (position.getSecurityId() == securityId) {
            position.updatePrice(newPrice);
            return;
        }
    }
}

double Portfolio::getPrice(int securityId) const{
    for (Position position : positions) {
        if (position.getSecurityId() == securityId) {
            return position.getValue()/position.getQuantity();
        }
    }
    return 0.0;
}

void Portfolio::updatePosition(int securityId,double quantity){
    for (Position position : positions) {
        if (position.getSecurityId() == securityId) {
            position.addQuantity(quantity);
        }
    }
}

void Portfolio::updateCash(double cashPrice) {
    cash += cashPrice;
}