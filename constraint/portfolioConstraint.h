#pragma once
#include "ConstraintEngine.h"

class MaxSecurityWeightConstraint : public Constraint {
private:
    double maxWeight;

public:
    explicit MaxSecurityWeightConstraint(double maxWeight);

    std::vector<ConstraintViolation>
    evaluate(const Portfolio& portfolio,const std::vector<Trade>& trades) const override;
};

class MaxSectorExposure : public Constraint{
private:
    double maxExposure;

public:
    explicit MaxSectorExposure(double maxExposure);

    std::vector<ConstraintViolation>
    evaluate(const Portfolio& portfolio,const std::vector<Trade>& trades) const override;
};

class MinimumCash : public Constraint{
     private:
    double minCash;

public:
    explicit MinimumCash(double minCash);

    std::vector<ConstraintViolation>
    evaluate(const Portfolio& portfolio,const std::vector<Trade>& trades) const override;
};