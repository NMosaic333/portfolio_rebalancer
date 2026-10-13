#include <cmath>
#include <stdexcept>

#include "variance.h"

double calculateVariance(const Eigen::VectorXd& returns)
{
    if (returns.size() < 2) {
        throw std::invalid_argument(
            "At least two returns are required"
        );
    }

    double mean = meanReturn(returns);

    double variance = 0.0;

    for (int i = 0; i < returns.size(); i++) {

        double difference = returns(i) - mean;

        variance += difference * difference;
    }

    variance /= (returns.size() - 1);

    return variance;
}

double calculateVolatility(const Eigen::VectorXd& returns)
{
    double variance = calculateVariance(returns);

    return std::sqrt(variance);
}

double calculateAnnualVolatility(
    const Eigen::VectorXd& dailyReturns
)
{
    double variance = calculateVariance(dailyReturns);

    return std::sqrt(252.0 * variance);
}

Eigen::MatrixXd covarianceMatrix(
    const Eigen::MatrixXd& returns
){
    Eigen::VectorXd means= meanReturn(returns);
    Eigen::MatrixXd centered = returns.rowwise() - means.transpose();

    Eigen::MatrixXd covariance = (centered.transpose() * centered)/ (returns.rows() - 1);
    return covariance;
}

double portfolioVolatility(
    const Eigen::MatrixXd& covariance,const Eigen::VectorXd& weights
){
    double portfolioVariance = weights.transpose() * covariance * weights;
    double portfolioVolatility = sqrt(portfolioVariance);
    return portfolioVolatility;
}

double annualPortfolioVolatility(
    const Eigen::MatrixXd& covariance,const Eigen::VectorXd& weights
){
    double dailyVolatility = portfolioVolatility(covariance,weights);
    double annualVolatility = std::sqrt(252.0) * dailyVolatility;
    return annualVolatility;
}