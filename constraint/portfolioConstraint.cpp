#include "portfolioConstraint.h"

MaxSecurityWeightConstraint::MaxSecurityWeightConstraint(double maxWeight)
    : maxWeight(maxWeight)
{
}

std::vector<ConstraintViolation>
MaxSecurityWeightConstraint::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const
{
    std::vector<ConstraintViolation> violations;

    for (const auto& position : portfolio.getPositions()) {

        double weight =
            portfolio.getWeight(position.getSecurityId());

        if (weight > maxWeight) {

            violations.push_back({
                ConstraintType::MaxSecurityWeight,
                position.getSecurity().getName(),
                weight,
                maxWeight,
                weight - maxWeight
            });
        }
    }

    return violations;
}


MaxSectorExposure::MaxSectorExposure(double maxExposure)
    : maxExposure(maxExposure)
{
}

std::vector<ConstraintViolation>
MaxSectorExposure::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const
{
    std::vector<ConstraintViolation> violations;

    std::unordered_map<std::string, double> sectorWeights;

    // Calculate total weight per sector
    for (const auto& position : portfolio.getPositions()) {

        double weight =
            portfolio.getWeight(position.getSecurityId());

        sectorWeights[
            position.getSecurity().getSector()
        ] += weight;
    }

    // Check sector limits
    for (const auto& [sector, weight] : sectorWeights) {

        if (weight > maxExposure) {

            violations.push_back({
                ConstraintType::MaxSectorExposure,
                sector,
                weight,
                maxExposure,
                weight - maxExposure
            });
        }
    }

    return violations;
}


MinimumCash::MinimumCash(double minCash)
    : minCash(minCash)
{
}

std::vector<ConstraintViolation>
MinimumCash::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const
{
    std::vector<ConstraintViolation> violations;

    double cash = portfolio.getCash();

    if (cash < minCash) {

        violations.push_back({
            ConstraintType::MinimumCash,
            "Cash",
            cash,
            minCash,
            minCash - cash
        });
    }

    return violations;
}