#include <Eigen/Dense>
#include "returns.h"

double calculateVariance(const Eigen::VectorXd& returns);
double calculateVolatility(const Eigen::VectorXd& returns);
double calculateAnnualVolatility(const Eigen::VectorXd& dailyReturns);
Eigen::MatrixXd covarianceMatrix(const Eigen::MatrixXd& returns);
double portfolioVolatility(const Eigen::MatrixXd& covariance,const Eigen::VectorXd& weights);
double annualPortfolioVolatility(const Eigen::MatrixXd& covariance,const Eigen::VectorXd& weights);