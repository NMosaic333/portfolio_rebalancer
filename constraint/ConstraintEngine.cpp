#include "ConstraintEngine.h"

double getTurnover(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) {
    double turnover=0.0;
    for(const auto& trade:trades){
        turnover += trade.getValue();
    }
    turnover/=portfolio.getPortfolioValue();
    return turnover;
}

std::vector<ConstraintViolation> ConstraintEngine::evaluate(const Portfolio& portfolio,const std::vector<Trade>& trades) const{
    std::vector<ConstraintViolation> violations;

    for (const auto& constraint : constraints) {

        auto result = constraint->evaluate(portfolio,trades);

        violations.insert(
            violations.end(),
            result.begin(),
            result.end()
        );
    }

    return violations;
}

void ConstraintEngine::addConstraint( std::unique_ptr<Constraint> constraint) {
    constraints.push_back(std::move(constraint));
}
