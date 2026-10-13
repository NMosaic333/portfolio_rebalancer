#include "OptimizerEngine.h"
#include <cmath>
#include <algorithm>

Trade* OptimizerEngine::findTrade(
    std::vector<Trade>& trades,
    const std::string& security
) const {
    for (auto& trade : trades) {
        if (trade.securityName == security) {
            return &trade;
        }
    }

    return nullptr;
}

void OptimizerEngine::getFinalPortfolio(
    Portfolio& portfolio,
    const std::vector<Trade>& trades
) const {

    for (const auto& trade : trades) {

        double quantity = trade.quantity;
        double tradeValue = trade.quantity * trade.price;

        if (trade.side == TradeSide::SELL) {
            quantity *= -1;
            portfolio.updateCash(tradeValue);
        }
        else if (trade.side == TradeSide::BUY) {
            portfolio.updateCash(-tradeValue);
        }

        portfolio.updatePosition(trade.securityId, quantity);
    }
}

OptimizationResult OptimizerEngine::createResult(
    const Portfolio& portfolio,
    const std::vector<Trade>& trades,
    const Portfolio& finalPortfolio
) const {
    std::vector<Trade> finalTrades=trades;
    double turnover = getTurnover(finalPortfolio,finalTrades);

    double trackingError=0.0;
    std::vector<Position> finalPositions = finalPortfolio.getPositions();
    for(const auto& position: finalPositions){
        double diff = position.getQuantity() - portfolio.getWeight(position.getSecurityId());
        trackingError += (diff * diff);
    }

    trackingError = std::sqrt(trackingError);

    constexpr double COST_RATE = 0.002;
    double totalTradeValue = turnover * finalPortfolio.getPortfolioValue();
    double transactionCost=totalTradeValue*COST_RATE;

    return OptimizationResult({
        finalPortfolio,
        finalTrades,
        turnover,
        trackingError,
        transactionCost
    });
}

void OptimizerEngine::repairViolation(
    const ConstraintViolation& violation,
    Portfolio& portfolio,
    std::vector<Trade>& trades
) const {

    switch (violation.type) {

        case ConstraintType::RestrictedSecurity:
            repairRestrictedSecurity(violation, trades);
            break;

        case ConstraintType::MinimumTradeValue:
            repairMinimumTradeValue(violation, trades);
            break;

        case ConstraintType::MaxSecurityWeight:
            repairMaxSecurityWeight(violation, portfolio, trades);
            break;

        case ConstraintType::MaxSectorExposure:
            repairMaxSectorExposure(violation, portfolio, trades);
            break;

        case ConstraintType::MinimumCash:
            repairMinimumCash(violation, portfolio, trades);
            break;

        case ConstraintType::MaxTurnover:
            repairMaxTurnover(violation, trades);
            break;
    }
}

OptimizationResult OptimizerEngine::optimize(
    const Portfolio& currentPortfolio,
    const std::vector<Trade>& proposedTrades,
    const ConstraintEngine& constraintEngine
) const {

    Portfolio portfolio = currentPortfolio;
    std::vector<Trade> trades = proposedTrades;

    constexpr int MAX_ITERATIONS = 20;

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        getFinalPortfolio(portfolio,trades);

        auto violations =
            constraintEngine.evaluate(portfolio, trades);

        if (violations.empty()) {
            return OptimizerEngine::createResult(currentPortfolio, trades, portfolio);
        }

        for (const auto& violation : violations) {
            repairViolation(violation, portfolio, trades);
        }

    }
    getFinalPortfolio(portfolio,trades);
    return OptimizerEngine::createResult(currentPortfolio, trades, portfolio);
}

void OptimizerEngine:: repairMaxTurnover(
    const ConstraintViolation& violation,
    std::vector<Trade>& trades
) const {
    double turnover = violation.actualValue;
    double maxTurnover = violation.limit;
    double difference = maxTurnover / turnover;
    for(auto& trade:trades){
        trade.quantity = trade.quantity * difference;
    }
}

void OptimizerEngine::repairRestrictedSecurity(
    const ConstraintViolation& violation,
    std::vector<Trade>& trades
) const {
    trades.erase(
        std::remove_if(
            trades.begin(),
            trades.end(),
            [&violation](const Trade& trade) {
                return trade.securityName == violation.security;
            }
        ),
        trades.end()
    );
}

void OptimizerEngine::repairMinimumTradeValue(
    const ConstraintViolation& violation,
    std::vector<Trade>& trades
) const{
    trades.erase(
        std::remove_if(
            trades.begin(),
            trades.end(),
            [&violation](const Trade& trade) {
                return trade.securityName == violation.security;
            }
        ),
        trades.end()
    );
}

void OptimizerEngine::repairMinimumCash(
    const ConstraintViolation& violation,
    const Portfolio& portfolio,
    std::vector<Trade>& trades
) const {

    double shortfall = violation.excess;
    double sellCash = 0.0;
    double buyCash = 0.0;

    for (auto& trade : trades) {
        if (trade.side == TradeSide::BUY) {
            buyCash += trade.getValue();
        }
        else {
            sellCash += trade.getValue();
        }
    }

    double buyReduction = std::min(shortfall, buyCash);

    double buyReductionRatio = 1.0;

    if (buyCash > 0.0) {
        buyReductionRatio = 1.0 - buyReduction / buyCash;
    }

    double remainingShortfall = shortfall - buyReduction;

    double sellReductionRatio = 1.0;

    if (sellCash > 0.0 && remainingShortfall > 0.0) {
        sellReductionRatio =
            1.0 + remainingShortfall / sellCash;
    }

    for (auto& trade : trades) {

        if (trade.side == TradeSide::BUY) {
            trade.quantity *= buyReductionRatio;
        }
        else {
            trade.quantity *= sellReductionRatio;
        }
    }
}

void OptimizerEngine::repairMaxSectorExposure(
    const ConstraintViolation& violation,
    const Portfolio& portfolio,
    std::vector<Trade>& trades
) const{
    double ratio = (violation.excess / violation.actualValue);
    for (const auto& position : portfolio.getPositions()) {
        if (position.getSecurity().getSector() != violation.security) {
            continue;
        }

        double sellQuantity =
            position.getQuantity() * ratio;

        if (sellQuantity <= 0.0) {
            continue;
        }

        Trade* trade = findTrade(trades, position.getSecurity().getName());

        if (trade == nullptr) {

            trades.push_back({
                position.getSecurityId(),
                position.getSecurity().getName(),
                TradeSide::SELL,
                sellQuantity,
                position.getSecurity().getCurrentPrice()
            });

        }
        else if (trade->side == TradeSide::SELL) {

            trade->quantity += sellQuantity;

        }
        else if (trade->side == TradeSide::BUY) {

            if (trade->quantity >= sellQuantity) {
                trade->quantity -= sellQuantity;
            }
            else {
                trade->side = TradeSide::SELL;
                trade->quantity = sellQuantity - trade->quantity;
            }
        }
    }
}

void OptimizerEngine::repairMaxSecurityWeight(
    const ConstraintViolation& violation,
    const Portfolio& portfolio,
    std::vector<Trade>& trades
) const{
    Trade* trade = findTrade(trades, violation.security);

    for(const auto& position: portfolio.getPositions()){
            double sellQuantity =
            position.getQuantity() *
            (violation.excess / violation.actualValue);

        if (trade == nullptr) {

            trades.push_back({
                position.getSecurityId(),
                violation.security,
                TradeSide::SELL,
                sellQuantity,
                position.getSecurity().getCurrentPrice()
            });

        }
        else if (trade->side == TradeSide::SELL) {

            trade->quantity += sellQuantity;

        }
        else if (trade->side == TradeSide::BUY) {

            if (trade->quantity >= sellQuantity) {
                trade->quantity -= sellQuantity;
            }
            else {
                trade->side = TradeSide::SELL;
                trade->quantity = sellQuantity - trade->quantity;
            }
        }
    }
}