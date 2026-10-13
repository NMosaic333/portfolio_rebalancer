#include "optimizer.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <tuple>


OptimizerEngine::OptimizerEngine(
    const OptimizerData& inputData
){
    data = inputData;
}

// {column, row, coefficient}
using ConstraintEntry =
    std::tuple<OSQPInt, OSQPInt, OSQPFloat>;

void manageMinCash(
    int n,
    ConstraintData& cons,
    OSQPFloat minCash,
    OSQPFloat currentWeightSum,
    std::vector<ConstraintEntry>& list
){
    int row = static_cast<int>(cons.l.size());

    for(int i = 0; i < n; i++){
        list.push_back({i, row, 1.0});
    }

    cons.l.push_back(-OSQP_INFTY);

    cons.u.push_back(
        1.0 - minCash - currentWeightSum
    );
}

void manageMaxSecurityRestrictedSecurity(
    int n,
    ConstraintData& cons,
    OSQPFloat maxValue,
    const std::vector<OSQPInt>& restr,
    const std::vector<OSQPFloat>& weights,
    std::vector<ConstraintEntry>& list
){
    int row = static_cast<int>(cons.l.size());

    int k = 0;
    int m=static_cast<int>(restr.size());

    for(int i = 0; i < n; i++){

        list.push_back({i, row + i, 1.0});

        if(k < m &&
           i == restr[k])
        {
            cons.l.push_back(0.0);
            cons.u.push_back(0.0);

            k++;
        }
        else
        {
            cons.l.push_back(-weights[i]);

            cons.u.push_back(
                maxValue - weights[i]
            );
        }
    }
}

void manageAbsoluteTrades(
    int n,
    ConstraintData& cons,
    std::vector<ConstraintEntry>& list
){
    int row = static_cast<int>(cons.l.size());

    for(int i = 0; i < n; i++){

        int x = i;
        int t = n + i;

        list.push_back({x, row, 1.0});
        list.push_back({t, row, -1.0});

        cons.l.push_back(-OSQP_INFTY);
        cons.u.push_back(0.0);

        row++;

        list.push_back({x, row, -1.0});
        list.push_back({t, row, -1.0});

        cons.l.push_back(-OSQP_INFTY);
        cons.u.push_back(0.0);

        row++;
    }
}

void manageMaxTurnover(
    int n,
    ConstraintData& cons,
    OSQPFloat maxTurnover,
    std::vector<ConstraintEntry>& list
){
    int row = static_cast<int>(cons.l.size());

    for(int i = 0; i < n; i++){

        int t = n + i;

        list.push_back({t, row, 1.0});
    }

    cons.l.push_back(-OSQP_INFTY);
    cons.u.push_back(maxTurnover);
}

void manageMaxSectorWeights(
    int n,
    int m,
    ConstraintData& cons,
    OSQPFloat maxWeight,
    const std::vector<OSQPInt>& sector,
    const std::vector<OSQPFloat>& weights,
    std::vector<ConstraintEntry>& list
){
    int row = static_cast<int>(cons.l.size());

    int k = 0;
    std::vector<OSQPFloat>sectorWeights(m,0.0);

    for(int i = 0; i < n; i++){
        list.push_back({i, row + sector[i], 1.0});
        sectorWeights[sector[i]]+=weights[i];
    }

    for(int i = 0; i < m; i++){
        cons.l.push_back(-OSQP_INFTY);

        cons.u.push_back(
            maxWeight - sectorWeights[i]
        );
    }
}

void applyMinimumTrade(
    int n,
    std::vector<double>& trades,
    double minTradeValue
){
    for (OSQPInt i = 0; i < n; ++i) {
        double trade = trades[i];

        if (std::abs(trade) < minTradeValue) {
            trades[i] = 0.0;
        }
    }
}

void OptimizerEngine::buildObjective(
    OSQPInt n,
    ObjectiveData& obj
){
    const OSQPInt numVars = 2 * n;

    std::vector<OSQPFloat> cw =
        data.getCurrentWeights();

    std::vector<OSQPFloat> tw =
        data.getTargetWeights();

    // --------------------------------------------------------
    // P = diagonal matrix
    //
    // x variables: 2.0
    // t variables: 0.0
    //
    // We ONLY store the non-zero x diagonal entries.
    // --------------------------------------------------------

    obj.P_x.clear();
    obj.P_i.clear();
    obj.P_p.clear();

    obj.P_x.resize(n);
    obj.P_i.resize(n);
    obj.P_p.resize(numVars + 1);

    // x columns
    for (OSQPInt i = 0; i < n; i++) {

        obj.P_x[i] = 2.0;
        obj.P_i[i] = i;

        obj.P_p[i] = i;
    }

    // End of x columns
    obj.P_p[n] = n;

    // t columns are empty
    for (OSQPInt i = n + 1; i <= numVars; i++) {
        obj.P_p[i] = n;
    }

    obj.P.m = numVars;
    obj.P.n = numVars;

    obj.P.p = obj.P_p.data();
    obj.P.i = obj.P_i.data();
    obj.P.x = obj.P_x.data();

    obj.P.nzmax = n;
    obj.P.nz = -1;
    obj.P.owned = 0;


    // --------------------------------------------------------
    // q
    // --------------------------------------------------------

    obj.q.clear();
    obj.q.resize(numVars, 0.0);

    for (OSQPInt i = 0; i < n; i++) {

        obj.q[i] =
            -2.0 * (tw[i] - cw[i]);
    }

    // t variables remain 0
}

void OptimizerEngine::buildConstraints(
    OSQPInt n,
    ConstraintData& cons
){
    cons.A_x.clear();
    cons.A_i.clear();
    cons.A_p.clear();

    cons.l.clear();
    cons.u.clear();

    const OSQPInt numVars = 2 * n;

    std::vector<OSQPFloat> currentWeights =
        data.getCurrentWeights();


    OSQPFloat currentWeightSum = 0.0;

    for(const auto& weight : currentWeights){
        currentWeightSum += weight;
    }

    std::vector<ConstraintEntry> list;


    Constraints constraints =
        data.getConstraints();

    manageMinCash(
        n,
        cons,
        constraints.minCash,
        currentWeightSum,
        list
    );

    manageMaxSecurityRestrictedSecurity(
        n,
        cons,
        constraints.maxSecurityWeight,
        constraints.restrictedSecurities,
        currentWeights,
        list
    );

    manageAbsoluteTrades(
        n,
        cons,
        list
    );

    manageMaxTurnover(
        n,
        cons,
        constraints.maxTurnover,
        list
    );

    manageMaxSectorWeights(
        n,
        data.getTotalSectors(),
        cons,
        constraints.maxSectorWeight,
        data.getSecuritiesSector(),
        currentWeights,
        list
    );

    std::sort(
        list.begin(),
        list.end(),
        [](const ConstraintEntry& a,
        const ConstraintEntry& b)
        {
            if (std::get<0>(a) != std::get<0>(b))
                return std::get<0>(a) < std::get<0>(b);

            return std::get<1>(a) < std::get<1>(b);
        }
    );


    cons.A_x.clear();
    cons.A_i.clear();
    cons.A_p.clear();

    cons.A_p.resize(numVars + 1, 0);


    // Count entries in each column
    for (const auto& entry : list) {

        OSQPInt column = std::get<0>(entry);

        cons.A_p[column + 1]++;
    }


    // Convert counts to cumulative column pointers
    for (OSQPInt col = 0; col < numVars; col++) {

        cons.A_p[col + 1] +=
            cons.A_p[col];
    }


    // Store entries
    cons.A_x.resize(list.size());
    cons.A_i.resize(list.size());


    // Current position in each column
    std::vector<OSQPInt> position =
        cons.A_p;


    // Fill CSC
    for (const auto& entry : list) {

        OSQPInt column =
            std::get<0>(entry);

        OSQPInt row =
            std::get<1>(entry);

        OSQPFloat value =
            std::get<2>(entry);

        OSQPInt index =
            position[column]++;

        cons.A_i[index] = row;
        cons.A_x[index] = value;
    }

    // Set OSQP matrix dimensions
    cons.A.m =
        static_cast<OSQPInt>(cons.l.size());

    cons.A.n =
        numVars;

    // Point OSQP to CSC data
    cons.A.p =
        cons.A_p.data();

    cons.A.i =
        cons.A_i.data();

    cons.A.x =
        cons.A_x.data();

    cons.A.nzmax =
        static_cast<OSQPInt>(cons.A_x.size());

    cons.A.nz = -1;
    cons.A.owned = 0;
}


OptimizationResult OptimizerEngine::solve()
{
    const OSQPInt n = data.getTotalSecurities();
    const OSQPInt numVars = 2 * n;

    buildObjective(n, objective);
    buildConstraints(n, constraintData);

    const OSQPInt m =
        static_cast<OSQPInt>(constraintData.l.size());

    OSQPSettings settings{};
    osqp_set_default_settings(&settings);

    settings.verbose = 0;

    OSQPSolver* solver = nullptr;

    OSQPInt status = osqp_setup(
        &solver,
        &objective.P,
        objective.q.data(),
        &constraintData.A,
        constraintData.l.data(),
        constraintData.u.data(),
        m,
        numVars,
        &settings
    );

    if (status != 0 || solver == nullptr) {
        result.isFeasible = false;
        return result;
    }

    status = osqp_solve(solver);

    if (status == 0 &&
        solver->solution != nullptr &&
        solver->solution->x != nullptr)
    {
        result.trades.resize(n);
        result.finalWeights.resize(n);

        const auto& currentWeights =
            data.getCurrentWeights();

        // Extract optimized trades.
        for (OSQPInt i = 0; i < n; ++i) {
            result.trades[i] =
                solver->solution->x[i];
        }

        for(auto& trade:result.trades ){
            std::cout<<trade<<std::endl;
        }

        // Apply minimum trade value threshold.
        applyMinimumTrade(n,result.trades,data.getConstraints().minTradeValue);

        // Recalculate final portfolio weights.
        for (OSQPInt i = 0; i < n; ++i) {
            result.finalWeights[i] =
                currentWeights[i] + result.trades[i];
        }

        result.isFeasible = true;
    }
    else {
        result.isFeasible = false;
    }

    osqp_cleanup(solver);

    return result;
}

std::vector<OSQPFloat>
OptimizerEngine::getOptimizedWeights() const
{
    return result.finalWeights;
}

bool OptimizerEngine::validateSolution() const
{
    if(!result.isFeasible)
        return false;


    const auto& weights =
        result.finalWeights;


    if(weights.empty())
        return false;


    constexpr OSQPFloat tolerance = 1e-6;


    OSQPFloat sum = 0.0;


    for(OSQPFloat weight : weights){

        if(weight < -tolerance)
            return false;

        if(weight > 1.0 + tolerance)
            return false;


        sum += weight;
    }

    if(sum > 1.0 + tolerance)
        return false;


    return true;
}