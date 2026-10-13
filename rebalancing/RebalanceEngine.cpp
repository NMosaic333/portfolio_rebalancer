#include "RebalanceEngine.h"
#include <cmath>

TargetAllocation::TargetAllocation(
    const std::vector<Target>& targetPortfolio
) : targetPortfolio(targetPortfolio) {}

const std::vector<Target>& TargetAllocation::getTargetPortfolio() const {
    return targetPortfolio;
}

std::vector<Trade> RebalanceEngine::evaluate(
    const Portfolio& portfolio,
    const TargetAllocation& targetAllocation
) const {

    std::vector<Trade> proposedTrades;

    const std::vector<Target>& targets =
        targetAllocation.getTargetPortfolio();

    double totalValue = portfolio.getTotalValue();

    for (const auto& target : targets) {

        double currentWeight =
            portfolio.getWeight(target.securityId);

        double delta =
            target.targetWeight - currentWeight;

        if (delta == 0.0) {
            continue;
        }

        double securityPrice =
            portfolio.getPrice(target.securityId);

        double quantity =
            std::abs(delta * totalValue / securityPrice);

        if (delta < 0) {

            proposedTrades.push_back({
                target.securityId,
                target.securityName,
                TradeSide::SELL,
                quantity,
                securityPrice
            });

        } else {

            proposedTrades.push_back({
                target.securityId,
                target.securityName,
                TradeSide::BUY,
                quantity,
                securityPrice
            });
        }
    }

    return proposedTrades;
}