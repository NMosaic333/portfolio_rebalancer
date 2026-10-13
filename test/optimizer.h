#include "prepoptimizer.h"

struct ObjectiveData {
    std::vector<OSQPFloat> P_x;
    std::vector<OSQPInt> P_i;
    std::vector<OSQPInt> P_p;

    OSQPCscMatrix P{};

    std::vector<OSQPFloat> q;
};

struct ConstraintData {
    std::vector<OSQPFloat> A_x;
    std::vector<OSQPInt> A_i;
    std::vector<OSQPInt> A_p;

    OSQPCscMatrix A{};

    std::vector<OSQPFloat> l;
    std::vector<OSQPFloat> u;
};

struct OptimizationResult {
    std::vector<OSQPFloat> finalWeights;
    std::vector<OSQPFloat> trades;

    bool isFeasible = false;
};

class OptimizerEngine {
private:
    OptimizerData data;

    ObjectiveData objective;
    ConstraintData constraintData;

    OptimizationResult result;

public:
    explicit OptimizerEngine(
        const OptimizerData& inputData
    );

    void buildObjective(
        OSQPInt n,
        ObjectiveData& obj
    );

    void buildConstraints(
        OSQPInt n,
        ConstraintData& cons
    );

    OptimizationResult solve();

    std::vector<OSQPFloat> getOptimizedWeights() const;

    bool validateSolution() const;
};