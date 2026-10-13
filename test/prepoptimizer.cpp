#include "prepoptimizer.h"
#include <cmath>
#include <iostream>

void OptimizerData::setCurrentWeights(
    const std::vector<OSQPFloat>& weights
){
    currentWeights = weights;
    totalSecurities = static_cast<OSQPInt>(currentWeights.size());
}

void OptimizerData::setTargetWeights(
    const std::vector<OSQPFloat>& weights
){
    targetWeights = weights;
}

void OptimizerData::setSecuritySector(
    const std::vector<OSQPInt>& weights,
    int sectors
){
    securitySector = weights;
    totalSectors = sectors ;
}

void OptimizerData::setMaxSecurityWeight(OSQPFloat value){
    constraints.maxSecurityWeight = value;
}

void OptimizerData::setMaxSectorWeight(OSQPFloat value){
    constraints.maxSectorWeight = value;
}
void OptimizerData::setMaxTurnover(OSQPFloat value){
    constraints.maxTurnover = value;
}
void OptimizerData::setMinTradeValue(OSQPFloat value){
    constraints.minTradeValue=value;
}
void OptimizerData::setMinCash(OSQPFloat value){
    constraints.minCash=value;
}
void OptimizerData::setRestrictedSecurities(
    const std::vector<OSQPInt>& securities
){
    constraints.restrictedSecurities = securities;
}

const std::vector<OSQPFloat>&
OptimizerData::getCurrentWeights() const{
    return currentWeights;
}

const std::vector<OSQPFloat>&
OptimizerData::getTargetWeights() const{
    return targetWeights;
}

const std::vector<OSQPInt>&
OptimizerData::getSecuritiesSector() const{
    return securitySector;
}

OSQPInt OptimizerData::getTotalSecurities() const{
    return totalSecurities;
}

OSQPInt OptimizerData::getTotalSectors() const{
    return totalSectors;
}

const Constraints& OptimizerData::getConstraints() const{
    return constraints;
}

bool OptimizerData::isReady() const
{
    if (currentWeights.empty())
        return false;

    if (currentWeights.size() != targetWeights.size())
        return false;

    if (totalSecurities != static_cast<OSQPInt>(currentWeights.size()))
        return false;

    for (OSQPFloat weight : currentWeights) {
        if (weight < 0.0 || weight >1.0)
            return false;
    }

    for (OSQPFloat weight : targetWeights) {
        if (weight < 0.0 || weight >1.0)
            return false;
    }

    OSQPFloat currentSum = 0.0;
    OSQPFloat targetSum = 0.0;

    for (OSQPFloat weight : currentWeights)
        currentSum += weight;

    for (OSQPFloat weight : targetWeights)
        targetSum += weight;

    if (currentSum > 1.0)
        return false;

    if (targetSum > 1.0)
        return false;

    if (constraints.maxSecurityWeight < 0.0 ||
        constraints.maxSecurityWeight > 1.0)
        return false;

    if (constraints.maxSectorWeight < 0.0 ||
        constraints.maxSectorWeight > 1.0)
        return false;

    if (constraints.maxTurnover < 0.0 ||
        constraints.maxTurnover > 2.0)
        return false;

    if (constraints.minTradeValue < 0.0)
        return false;

    if (constraints.minCash < 0.0 ||
        constraints.minCash > 1.0)
        return false;

    return true;
}