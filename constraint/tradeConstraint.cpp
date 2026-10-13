#include "tradeConstraint.h"

MaxTurnover::MaxTurnover(double maxTurnover):maxTurnover(maxTurnover){}

std::vector<ConstraintViolation> MaxTurnover::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const 
{
    std::vector<ConstraintViolation> violations;
    double turnover = getTurnover(portfolio,trades);
    
    if(turnover>maxTurnover){
        violations.push_back({
            ConstraintType::MaxTurnover,
            "Portfolio",
            turnover,
            maxTurnover,
            turnover-maxTurnover
        });
    }
    return violations;
}

RestrictedSecurity::RestrictedSecurity(const std::unordered_set<std::string>& restricted):restricted(restricted){}

std::vector<ConstraintViolation> RestrictedSecurity::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const {
    std::vector<ConstraintViolation> violations;
    for(const auto& trade: trades){
        if(restricted.count(trade.securityName)){
            violations.push_back({
                ConstraintType::RestrictedSecurity,
                trade.securityName,
                trade.quantity,
                0.0,
                trade.quantity
            });
        }
    }
    return violations;
}


MinimumTradeValue::MinimumTradeValue(double minTradeValue):minTradeValue(minTradeValue){}

std::vector<ConstraintViolation> MinimumTradeValue::evaluate(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades
) const {
    std::vector<ConstraintViolation> violations;
    for(const auto& trade :trades){
        double value = trade.getValue();
        if(value <minTradeValue){
            violations.push_back({
                ConstraintType::MinimumTradeValue,
                trade.securityName,
                value,
                minTradeValue,
                value - minTradeValue
            });
        }
    }
    return violations;
}