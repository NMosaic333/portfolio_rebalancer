#include <Eigen/Dense>

double calculateSharpeRatio(
    const Eigen::MatrixXd& returns,
    const Eigen::MatrixXd& covariance,
    const Eigen::VectorXd& weights,
    double annualRiskFreeRate
);

double beta(
    const Eigen::VectorXd& assetReturns,
    const Eigen::VectorXd& benchmarkReturns
);

double maximumDrawdown(
    const Eigen::VectorXd& portfolioValues
);

double historicalVaR(const Eigen::VectorXd& historicalReturns,double confidence);
