#pragma once

#include "../constraint/ConstraintEngine.h"

struct Target {
    int securityId;
    std::string securityName;
    double targetWeight;
};

class TargetAllocation {
private:
    std::vector<Target> targetPortfolio;

public:
    TargetAllocation(
        const std::vector<Target>& targetPortfolio
    );

    const std::vector<Target>& getTargetPortfolio() const;
};

class RebalanceEngine {
public:
    std::vector<Trade> evaluate(
        const Portfolio& portfolio,
        const TargetAllocation& targetAllocation
    ) const;
};