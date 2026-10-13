#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cmath>
#include "returns.h"
#include "variance.h"

double calculateSharpeRatio(
    const Eigen::MatrixXd& returns,
    const Eigen::MatrixXd& covariance,
    const Eigen::VectorXd& weights,
    double annualRiskFreeRate
){
    double portfolioReturn = annualizedPortfolioReturn(returns,weights);
    double portfolioVolatility = annualPortfolioVolatility(covariance,weights);
    double sharpe = (portfolioReturn - annualRiskFreeRate)/portfolioVolatility;
    return sharpe;
}

double beta(
    const Eigen::VectorXd& assetReturns,
    const Eigen::VectorXd& benchmarkReturns
){
    double meanAssetReturn = meanReturn(assetReturns);
    double meanBenchmarkReturn = meanReturn(benchmarkReturns);

    double covariance=0.0;
    for(int i=0;i<assetReturns.size();i++){
        covariance += ((assetReturns(i)-meanAssetReturn)*(benchmarkReturns(i)-meanBenchmarkReturn));
    }
    covariance /=(assetReturns.size()-1);
    double variance = calculateVariance(benchmarkReturns);
    double beta = covariance/variance;
    return beta;
}

double maximumDrawdown(
    const Eigen::VectorXd& portfolioValues
){
    double peak=portfolioValues(0),maxdrawdown=0.0;
    for(int i=1;i<portfolioValues.size();i++){
        if(peak<portfolioValues(i)) peak=portfolioValues(i);
        double currdrawdown = (portfolioValues(i)-peak)/peak;
        if(currdrawdown>maxdrawdown) maxdrawdown=currdrawdown;
    }
    return maxdrawdown;
}

double historicalVaR(
    const Eigen::VectorXd& historicalReturns,
    double confidence
){
    if (historicalReturns.size() == 0) {
        throw std::invalid_argument(
            "Historical returns cannot be empty"
        );
    }

    if (confidence <= 0.0 || confidence >= 1.0) {
        throw std::invalid_argument(
            "Confidence must be between 0 and 1"
        );
    }

    std::vector<double> sortedReturns(
        historicalReturns.data(),
        historicalReturns.data() + historicalReturns.size()
    );

    std::sort(sortedReturns.begin(), sortedReturns.end());

    double position =
        (1.0 - confidence) * (sortedReturns.size() - 1);

    int lower = static_cast<int>(std::floor(position));
    int upper = static_cast<int>(std::ceil(position));

    double percentile =
        sortedReturns[lower] +
        (position - lower) *
        (sortedReturns[upper] - sortedReturns[lower]);

    return -percentile;
}