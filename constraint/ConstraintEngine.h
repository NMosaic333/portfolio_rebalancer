#pragma once

#include <memory>
#include <unordered_set>
#include<unordered_map>
#include "../portfolio/Security.h"

enum class TradeSide {
    BUY,
    SELL
};

struct Trade {
    int securityId;
    std::string securityName;
    TradeSide side;
    double quantity;
    double price;

    double getValue() const {
        return quantity * price;
    }
};

double getTurnover(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
);

enum class ConstraintType {
    MaxTurnover,
    RestrictedSecurity,
    MinimumTradeValue,
    MinimumCash,
    MaxSectorExposure,
    MaxSecurityWeight
};

struct ConstraintViolation {
    ConstraintType type;
    std::string security;
    double actualValue;
    double limit;
    double excess;
};

class Constraint {
public:
    virtual ~Constraint() = default;

    virtual std::vector<ConstraintViolation> evaluate(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const = 0;

};

class ConstraintEngine {
private:
    std::vector<std::unique_ptr<Constraint>> constraints;

public:
    void addConstraint(
        std::unique_ptr<Constraint> constraint
    );

    std::vector<ConstraintViolation> evaluate(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const;
    
};