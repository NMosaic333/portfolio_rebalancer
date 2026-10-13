#include "optimizer.h"

#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    std::vector<OSQPFloat> currentWeights = {
        0.35,
        0.15,
        0.20
    };

    std::vector<OSQPInt> sector = {
        0,
        0,
        1
    };

    std::vector<OSQPFloat> targetWeights = {
        0.40,
        0.25,
        0.30
    };

    OptimizerData data;

    data.setCurrentWeights(currentWeights);
    data.setTargetWeights(targetWeights);
    data.setSecuritySector(sector,2);

    data.setMaxSecurityWeight(1.0);
    data.setMaxSectorWeight(1.0);
    data.setMaxTurnover(1.0);
    data.setMinTradeValue(0.06);
    data.setMinCash(0.0);
    data.setRestrictedSecurities({});

    if (!data.isReady())
    {
        std::cerr << "ERROR: OptimizerData is not ready.\n";
        return 1;
    }

    std::cout << "Optimizer data is ready.\n";

    OptimizerEngine optimizer(data);

    OptimizationResult result = optimizer.solve();

    if (!result.isFeasible)
    {
        std::cerr << "ERROR: Optimization failed.\n";
        return 1;
    }

    std::cout << "Optimization succeeded!\n\n";

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Trades:\n";

    for (size_t i = 0; i < result.trades.size(); ++i)
    {
        std::cout
            << "Security " << i
            << ": "
            << result.trades[i]
            << "\n";
    }

    std::cout << "\nFinal weights:\n";

    for (size_t i = 0; i < result.finalWeights.size(); ++i)
    {
        std::cout
            << "Security " << i
            << ": "
            << result.finalWeights[i]
            << "\n";
    }

    // ---------------------------------------------------------
    // Validate complete solution
    // ---------------------------------------------------------

    if (!optimizer.validateSolution())
    {
        std::cerr
            << "ERROR: Solution failed validation.\n";
        return 1;
    }

    std::cout
        << "Solution passed validation.\n";

    return 0;
}