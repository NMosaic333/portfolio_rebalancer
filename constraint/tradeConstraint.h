#pragma once
#include "ConstraintEngine.h"

class MaxTurnover : public Constraint {
private:
    double maxTurnover;

public:
    explicit MaxTurnover(double maxTurnover);

    std::vector<ConstraintViolation> evaluate(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const override;
};


class RestrictedSecurity : public Constraint {
private:
    std::unordered_set<std::string> restricted;

public:
    explicit RestrictedSecurity(
        const std::unordered_set<std::string>& restricted
    );

    std::vector<ConstraintViolation> evaluate(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const override;
};


class MinimumTradeValue : public Constraint {
private:
    double minTradeValue;

public:
    explicit MinimumTradeValue(double minTradeValue);

    std::vector<ConstraintViolation> evaluate(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const override;
};