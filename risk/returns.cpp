#include "returns.h"
#include <stdexcept>

Eigen::VectorXd calculateReturns(
    const Eigen::VectorXd& prices
){
    if (prices.size() < 2) {
        throw std::invalid_argument(
            "At least two prices are required"
        );
    }

    Eigen::VectorXd returns(prices.size() - 1);

    for (int i = 1; i < prices.size(); i++) {

        if (prices(i - 1) == 0) {
            throw std::invalid_argument(
                "Price cannot be zero"
            );
        }

        returns(i - 1) =
            (prices(i) - prices(i - 1))
            / prices(i - 1);
    }

    return returns;
}

double meanReturn(
    const Eigen::VectorXd& returns
){
    if (returns.size() == 0) {
        throw std::invalid_argument(
            "Returns vector cannot be empty"
        );
    }

    double mean = 0.0;

    for (int i = 0; i < returns.size(); i++) {
        mean += returns(i);
    }

    return mean / returns.size();
}

Eigen::MatrixXd calculateReturns(
    const Eigen::MatrixXd& prices
){
    if (prices.rows() < 2) {
        throw std::invalid_argument(
            "At least two prices are required"
        );
    }

    Eigen::MatrixXd returns(prices.rows() - 1,prices.cols());

    for (int i = 1; i < prices.rows(); i++) {
        for(int j=0;j<prices.cols();j++){
            if (prices(i - 1,j) == 0) {
                throw std::invalid_argument(
                    "Price cannot be zero"
                );
            }

            returns(i - 1,j) =
                (prices(i,j) - prices(i - 1,j))
                / prices(i - 1,j);
        }
    }

    return returns;
}

Eigen::VectorXd meanReturn(
    const Eigen::MatrixXd& returns
){
    if (returns.rows() == 0 || returns.cols() == 0) {
        throw std::invalid_argument(
            "Returns matrix cannot be empty"
        );
    }

    Eigen::VectorXd means(returns.cols());

    for (int i = 0; i < returns.cols(); i++) {

        double mean = 0.0;

        for (int j = 0; j < returns.rows(); j++) {
            mean += returns(j, i);
        }

        means(i) = mean / returns.rows();
    }

    return means;
}

double portfolioReturn(const Eigen::MatrixXd& returns, const Eigen::VectorXd& weights){
    Eigen::VectorXd dailyReturns = returns * weights;
    return meanReturn(dailyReturns);
}

double annualizedPortfolioReturn(const Eigen::MatrixXd& returns, const Eigen::VectorXd& weights){
    double averageDailyReturn=portfolioReturn(returns,weights);
    double annualPortfolioReturn = averageDailyReturn * 252.0;
    return annualPortfolioReturn;
}