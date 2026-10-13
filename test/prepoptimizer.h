#include "osqp.h"
#include <vector>

struct Constraints {
    OSQPFloat maxSecurityWeight = 1.0;
    OSQPFloat maxSectorWeight = 1.0;
    OSQPFloat maxTurnover = 1.0;
    OSQPFloat minTradeValue = 0.0;
    OSQPFloat minCash = 0.0;
    std::vector<OSQPInt> restrictedSecurities;
};

class OptimizerData {
private:
    std::vector<OSQPFloat> currentWeights;
    std::vector<OSQPFloat> targetWeights;
    std::vector<OSQPInt> securitySector;

    OSQPInt totalSecurities = 0;
    OSQPInt totalSectors = 0;
    Constraints constraints;

public:
    void setCurrentWeights(
        const std::vector<OSQPFloat>& weights
    );

    void setTargetWeights(
        const std::vector<OSQPFloat>& weights
    );
    void setSecuritySector(
        const std::vector<OSQPInt>& weights,
        int sectors
    );


    void setMaxSecurityWeight(OSQPFloat value);
    void setMaxSectorWeight(OSQPFloat value);
    void setMaxTurnover(OSQPFloat value);
    void setMinTradeValue(OSQPFloat value);
    void setMinCash(OSQPFloat value);
    void setRestrictedSecurities(
        const std::vector<OSQPInt>& securities
    );

    const std::vector<OSQPFloat>&
    getCurrentWeights() const;

    const std::vector<OSQPFloat>&
    getTargetWeights() const;

    const std::vector<OSQPInt>&
    getSecuritiesSector() const;

    OSQPInt getTotalSecurities() const;
    OSQPInt getTotalSectors() const;

    const Constraints& getConstraints() const;

    bool isReady() const;
};
