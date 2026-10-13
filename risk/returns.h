#include <Eigen/Dense>

Eigen::VectorXd calculateReturns(
    const Eigen::VectorXd& prices
);

double meanReturn(
    const Eigen::VectorXd& returns
);

Eigen::MatrixXd calculateReturns(
    const Eigen::MatrixXd& prices
);

Eigen::VectorXd meanReturn(
    const Eigen::MatrixXd& returns
);

double portfolioReturn(const Eigen::MatrixXd& returns, const Eigen::VectorXd& weights);
double annualizedPortfolioReturn(const Eigen::MatrixXd& returns, const Eigen::VectorXd& weights);