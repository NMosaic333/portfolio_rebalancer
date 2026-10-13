#include "OptimizerEngine.h"
#include "../constraint/portfolioConstraint.h"
#include "../constraint/tradeConstraint.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

bool closeEnough(double left, double right) {
    return std::abs(left - right) < 1e-9;
}

const Trade& findTrade(
    const std::vector<Trade>& trades,
    const std::string& securityName
) {
    for (const auto& trade : trades) {
        if (trade.securityName == securityName) {
            return trade;
        }
    }

    assert(false && "Expected trade was not found");
    return trades.front();
}

bool hasTrade(
    const std::vector<Trade>& trades,
    const std::string& securityName,
    TradeSide side
) {
    for (const auto& trade : trades) {
        if (trade.securityName == securityName &&
            trade.side == side) {
            return true;
        }
    }

    return false;
}

double totalBuyValue(
    const std::vector<Trade>& trades
) {
    double total = 0.0;

    for (const auto& trade : trades) {
        if (trade.side == TradeSide::BUY) {
            total += trade.getValue();
        }
    }

    return total;
}

double totalSellValue(
    const std::vector<Trade>& trades
) {
    double total = 0.0;

    for (const auto& trade : trades) {
        if (trade.side == TradeSide::SELL) {
            total += trade.getValue();
        }
    }

    return total;
}

double sectorValue(
    const Portfolio& portfolio,
    const std::string& sector
) {
    double value = 0.0;

    for (const auto& position : portfolio.getPositions()) {

        if (position.getSecurity().getSector() == sector) {
            value += position.getValue();
        }
    }

    return value;
}

double sectorExposure(
    const Portfolio& portfolio,
    const std::string& sector
) {
    double portfolioValue =
        portfolio.getPortfolioValue();

    if (portfolioValue == 0.0) {
        return 0.0;
    }

    return sectorValue(portfolio, sector)
           / portfolioValue;
}

} // namespace


int main() {

    // ============================================================
    // SECURITIES
    // ============================================================

    Security apple(
        1,
        "AAPL",
        "Apple",
        "Technology",
        100.0
    );

    Security microsoft(
        2,
        "MSFT",
        "Microsoft",
        "Technology",
        100.0
    );

    Security jpMorgan(
        3,
        "JPM",
        "JPMorgan",
        "Financials",
        100.0
    );


    // ============================================================
    // TEST 1
    // BASIC REBALANCING
    // ============================================================

    {
        Portfolio portfolio(
            1,
            "Basic rebalance",
            0.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 10.0)
            }
        );

        RebalanceEngine engine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.50},
            {2, "Microsoft", 0.50}
        });

        std::vector<Trade> trades =
            engine.evaluate(
                portfolio,
                targetAllocation
            );

        assert(trades.size() == 2);

        const Trade& appleTrade =
            findTrade(trades, "Apple");

        assert(appleTrade.side == TradeSide::SELL);
        assert(closeEnough(appleTrade.quantity, 5.0));
        assert(closeEnough(appleTrade.price, 100.0));
        assert(closeEnough(appleTrade.getValue(), 500.0));


        const Trade& microsoftTrade =
            findTrade(trades, "Microsoft");

        assert(microsoftTrade.side == TradeSide::BUY);
        assert(closeEnough(microsoftTrade.quantity, 5.0));
        assert(closeEnough(microsoftTrade.price, 100.0));
        assert(closeEnough(microsoftTrade.getValue(), 500.0));

        std::cout
            << "Test 1 passed: basic rebalancing\n";
    }


    // ============================================================
    // TEST 2
    // ALREADY BALANCED
    // ============================================================

    {
        Portfolio portfolio(
            2,
            "Already balanced",
            0.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 10.0)
            }
        );

        RebalanceEngine engine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 2.0 / 3.0},
            {2, "Microsoft", 1.0 / 3.0}
        });

        std::vector<Trade> trades =
            engine.evaluate(
                portfolio,
                targetAllocation
            );

        assert(trades.empty());

        std::cout
            << "Test 2 passed: already balanced\n";
    }


    // ============================================================
    // TEST 3
    // MAX SECURITY WEIGHT
    // ============================================================

    {
        Portfolio portfolio(
            3,
            "Security weight test",
            0.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 10.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.80},
            {2, "Microsoft", 0.20}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MaxSecurityWeightConstraint>(
                0.60
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        assert(
            result.portfolio.getWeight(1) <=
            0.60 + 1e-9
        );

        std::cout
            << "Test 3 passed: max security weight\n";
    }


    // ============================================================
    // TEST 4
    // MAX SECTOR EXPOSURE
    // ============================================================

    {
        Portfolio portfolio(
            4,
            "Sector exposure test",
            0.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 10.0),
                Position(jpMorgan, 10.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        /*
         * Portfolio:
         *
         * Apple       = 2000
         * Microsoft   = 1000
         * JPM         = 1000
         *
         * Technology = 3000 / 4000 = 75%
         * Financials  = 1000 / 4000 = 25%
         */

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.50},
            {2, "Microsoft", 0.25},
            {3, "JPMorgan", 0.25}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MaxSectorExposure>(
                0.60
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        double technologyExposure =
            sectorExposure(
                result.portfolio,
                "Technology"
            );

        assert(
            technologyExposure <=
            0.60 + 1e-9
        );

        std::cout
            << "Test 4 passed: max sector exposure\n";
    }


    // ============================================================
    // TEST 5
    // MINIMUM CASH
    // ============================================================

    {
        Portfolio portfolio(
            5,
            "Minimum cash test",
            1000.0,
            {
                Position(apple, 10.0),
                Position(microsoft, 10.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        /*
         * Portfolio:
         *
         * Cash       = 1000
         * Apple      = 1000
         * Microsoft  = 1000
         *
         * Total = 3000
         *
         * Target asks for:
         *
         * Apple      = 20%
         * Microsoft  = 80%
         *
         * This creates a BUY requirement.
         */

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.20},
            {2, "Microsoft", 0.80}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MinimumCash>(
                1000.0
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        assert(
            result.portfolio.getCash() >=
            1000.0 - 1e-9
        );

        std::cout
            << "Test 5 passed: minimum cash\n";
    }


    // ============================================================
    // TEST 6
    // RESTRICTED SECURITY
    // ============================================================

    {
        Portfolio portfolio(
            6,
            "Restricted security test",
            1000.0,
            {
                Position(apple, 10.0),
                Position(microsoft, 10.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.20},
            {2, "Microsoft", 0.80}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<RestrictedSecurity>(
                std::unordered_set<std::string>{
                    "Microsoft"
                }
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        assert(
            !hasTrade(
                result.trades,
                "Microsoft",
                TradeSide::BUY
            )
        );

        std::cout
            << "Test 6 passed: restricted security\n";
    }


    // ============================================================
    // TEST 7
    // MINIMUM TRADE VALUE
    // ============================================================

    {
        Portfolio portfolio(
            7,
            "Minimum trade value test",
            5000.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 20.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.51},
            {2, "Microsoft", 0.49}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MinimumTradeValue>(
                100.0
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        /*
         * Every remaining trade should either:
         *
         * 1. Have value >= minimum trade value
         * 2. Have been removed by the repair.
         */

        for (const auto& trade : result.trades) {
            assert(
                trade.getValue() >=
                100.0 - 1e-9
            );
        }

        std::cout
            << "Test 7 passed: minimum trade value\n";
    }


    // ============================================================
    // TEST 8
    // MAX TURNOVER
    // ============================================================

    {
        Portfolio portfolio(
            8,
            "Turnover test",
            5000.0,
            {
                Position(apple, 20.0),
                Position(microsoft, 20.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1,"Apple",0.90},
            {2, "Microsoft", 0.10}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MaxTurnover>(
                0.10
            )
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

        double turnover =
            getTurnover(
                portfolio,
                result.trades
            );

        assert(
            turnover <=
            0.10 + 1e-9
        );

        std::cout
            << "Test 8 passed: max turnover\n";
    }


    // ============================================================
    // TEST 9
    // ALL CONSTRAINTS TOGETHER
    // ============================================================

    {
        Portfolio portfolio(
            9,
            "Combined constraints",
            1000.0,
            {
                Position(apple, 10.0),
                Position(microsoft, 10.0),
                Position(jpMorgan, 10.0)
            }
        );

        RebalanceEngine rebalanceEngine;
        OptimizerEngine optimizerEngine;

        TargetAllocation targetAllocation(std::vector<Target>{
            {1, "Apple", 0.50},
            {2, "Microsoft", 0.30},
            {3, "JPMorgan", 0.20}
        });

        std::vector<Trade> proposedTrades =
            rebalanceEngine.evaluate(
                portfolio,
                targetAllocation
            );

        ConstraintEngine constraintEngine;

        constraintEngine.addConstraint(
            std::make_unique<MaxSecurityWeightConstraint>(0.60)
        );

        constraintEngine.addConstraint(
            std::make_unique<MaxSectorExposure>(0.60)
        );

        constraintEngine.addConstraint(
            std::make_unique<MinimumCash>(1000.0)
        );

        constraintEngine.addConstraint(
            std::make_unique<RestrictedSecurity>(
                std::unordered_set<std::string>{
                    "Microsoft"
                }
            )
        );

        constraintEngine.addConstraint(
            std::make_unique<MinimumTradeValue>(100.0)
        );

        constraintEngine.addConstraint(
            std::make_unique<MaxTurnover>(0.20)
        );

        OptimizationResult result =
            optimizerEngine.optimize(
                portfolio,
                proposedTrades,
                constraintEngine
            );

            std::cout << "\n--- Test 9 Result ---\n";

        std::cout << "Cash: "
                << result.portfolio.getCash()
                << "\n";

        std::cout << "Apple weight: "
                << result.portfolio.getWeight(1)
                << "\n";

        std::cout << "Technology exposure: "
                << sectorExposure(
                        result.portfolio,
                        "Technology"
                    )
                << "\n";

        std::cout << "Turnover: "
                << getTurnover(
                        portfolio,
                        result.trades
                    )
                << "\n";

        std::cout << "Trades:\n";

        for (const auto& trade : result.trades) {
            std::cout
                << (trade.side == TradeSide::BUY ? "BUY " : "SELL ")
                << trade.securityName
                << " value=" << trade.getValue()
                << "\n";
        }
        // Basic feasibility checks
        assert(
            result.portfolio.getCash() >=
            1000.0 - 1e-9
        );

        assert(
            result.portfolio.getWeight(1) <=
            0.60 + 1e-9
        );

        assert(
            !hasTrade(
                result.trades,
                "Microsoft",
                TradeSide::BUY
            )
        );

        assert(
            getTurnover(
                portfolio,
                result.trades
            ) <= 0.20 + 1e-9
        );

        for (const auto& trade : result.trades) {
            assert(
                trade.getValue() >=
                100.0 - 1e-9
            );
        }

        double technologyExposure =
            sectorExposure(
                result.portfolio,
                "Technology"
            );

        assert(
            technologyExposure <=
            0.60 + 1e-9
        );

        std::cout
            << "Test 9 passed: all constraints together\n";
    }
    // ============================================================
    // FINAL OUTPUT
    // ============================================================

    std::cout
        << "\nAll tests passed successfully!\n";

    return 0;
}