# Institutional Portfolio Rebalancing and Risk Engine

This C++20 project contains portfolio, constraint, rebalancing, optimization,
and risk-analysis components for an institutional portfolio workflow. The
repository currently provides executable test programs for the rebalancing
engine and the OSQP-based optimizer.

## Features

- Portfolio and security models with position and cash updates
- Portfolio and trade constraint evaluation
- Rebalancing and trade-repair logic
- Return, variance, volatility, covariance, Sharpe ratio, beta, and VaR
  calculations
- OSQP-backed constrained portfolio optimization
- PostgreSQL schema and an optional database-backed application entry point

## Repository layout

```text
constraint/       Constraint interfaces and implementations
portfolio/        Security, position, portfolio, and repository models
rebalancing/      Rebalancing engine and tests
risk/             Return and risk calculations
test/             OSQP optimizer implementation and test
eigen-5.0.0/      Vendored Eigen headers and license notices
osqp/             OSQP Git submodule
schema.sql        PostgreSQL schema
CMakeLists.txt    Root build configuration
```

## Dependencies

- CMake 3.18 or newer
- A C++20 compiler
- Eigen 5.0.0 headers in `eigen-5.0.0/`
- OSQP in `osqp/` for the optimizer tests
- libpqxx and PostgreSQL only when building the optional database application

The dependency directories are intentionally excluded from this initial
repository snapshot. Obtain the approved dependency versions separately and
place them at the paths above before configuring the full build. Preserve all
license and attribution files supplied with those dependencies.

```powershell
git clone <repository-url>
cd portfolio_rebalancer
# Add Eigen headers under .\eigen-5.0.0\
# Add OSQP under .\osqp\ when optimizer tests are needed
```

## Configure and build

Run these commands from the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

With Eigen present, the default build produces:

- `rebalancing_tests`

When OSQP is also present, it additionally produces:

- `osqp_optimizer_tests`

To configure a multi-configuration generator such as Visual Studio, use the
same commands and specify `--config Release` when building or testing.

## Run tests

```powershell
ctest --test-dir build --build-config Release --output-on-failure
```

## Database application

`schema.sql` defines the PostgreSQL tables used by the database-backed
portfolio repository. The application entry point is not built by default
because it requires an externally installed libpqxx package and a running
PostgreSQL database.

After installing libpqxx and configuring PostgreSQL, enable the optional
target:

```powershell
cmake -S . -B build-db -DPORTFOLIO_BUILD_DATABASE_APP=ON
cmake --build build-db --config Release
```

The connection string is currently defined in `main.cpp`; provide a suitable
local database configuration before running that executable. Do not commit
credentials or local environment files.
