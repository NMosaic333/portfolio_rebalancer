CREATE TABLE users (
    id SERIAL PRIMARY KEY
);

CREATE TABLE portfolios (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    user_id INT NOT NULL REFERENCES users(id),
    cash NUMERIC(20,2) NOT NULL DEFAULT 0
);

CREATE TABLE securities (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    ticker VARCHAR(10) UNIQUE NOT NULL,
    sector VARCHAR(100)
);

CREATE TABLE holdings (
    portfolio_id INT REFERENCES portfolios(id),
    security_id INT REFERENCES securities(id),
    quantity NUMERIC(20,6) NOT NULL,
    average_price NUMERIC(20,6),

    PRIMARY KEY (portfolio_id, security_id)
);

CREATE TABLE prices (
    security_id INT REFERENCES securities(id),
    timestamp TIMESTAMP NOT NULL,
    price NUMERIC(20,6) NOT NULL,

    PRIMARY KEY (security_id, timestamp)
);

CREATE TABLE rebalance_runs (
    id SERIAL PRIMARY KEY,
    portfolio_id INT NOT NULL REFERENCES portfolios(id),
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(20) NOT NULL
);

CREATE TABLE orders (
    id SERIAL PRIMARY KEY,
    portfolio_id INT NOT NULL REFERENCES portfolios(id),
    security_id INT NOT NULL REFERENCES securities(id),

    side VARCHAR(4) NOT NULL,

    quantity NUMERIC(20,6) NOT NULL,
    price NUMERIC(20,6),

    rebalance_run_id INT NOT NULL REFERENCES rebalance_runs(id),
    status VARCHAR(20) NOT NULL DEFAULT 'PENDING',

    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE constraints (
    id SERIAL PRIMARY KEY,
    portfolio_id INT NOT NULL REFERENCES portfolios(id),
    constraint_type VARCHAR(50) NOT NULL,
    parameter VARCHAR(100),
    value NUMERIC(20,6) NOT NULL
);

CREATE TABLE risk_metrics (
    id SERIAL PRIMARY KEY,
    portfolio_id INT NOT NULL REFERENCES portfolios(id),
    rebalance_run_id INT REFERENCES rebalance_runs(id),
    metric_type VARCHAR(20) NOT NULL,
    value NUMERIC(20,6) NOT NULL,
    calculated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);