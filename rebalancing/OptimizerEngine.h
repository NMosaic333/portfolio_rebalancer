#pragma once

#include "RebalanceEngine.h"

struct OptimizationResult {
    Portfolio portfolio;
    std::vector<Trade> trades;

    double turnover;

    double trackingError;
    double transactionCost;

};

class OptimizerEngine {
public:
    OptimizationResult optimize(
        const Portfolio& currentPortfolio,
        const std::vector<Trade>& proposedTrades,
        const ConstraintEngine& constraintEngine
    ) const;

private:

    void getFinalPortfolio(
        Portfolio& portfolio,
        const std::vector<Trade>& trades
    ) const;

    OptimizationResult createResult(
        const Portfolio& portfolio,
        const std::vector<Trade>& trades,
        const Portfolio& finalPortfolio
    ) const;

    void repairViolation(
        const ConstraintViolation& violation,
        Portfolio& portfolio,
        std::vector<Trade>& trades
    ) const;

    void repairMaxTurnover(
        const ConstraintViolation& violation,
        std::vector<Trade>& trades
    ) const;

    void repairRestrictedSecurity(
        const ConstraintViolation& violation,
        std::vector<Trade>& trades
    ) const;

    void repairMinimumTradeValue(
        const ConstraintViolation& violation,
        std::vector<Trade>& trades
    ) const;

    void repairMinimumCash(
        const ConstraintViolation& violation,
        const Portfolio& portfolio,
        std::vector<Trade>& trades
    ) const;

    void repairMaxSectorExposure(
        const ConstraintViolation& violation,
        const Portfolio& portfolio,
        std::vector<Trade>& trades
    ) const;

    void repairMaxSecurityWeight(
        const ConstraintViolation& violation,
        const Portfolio& portfolio,
        std::vector<Trade>& trades
    ) const;

    Trade* findTrade(
        std::vector<Trade>& trades,
        const std::string& security
    ) const;
};