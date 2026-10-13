cmake_minimum_required(VERSION 3.18)

project(portfolio_rebalancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PORTFOLIO_BUILD_DATABASE_APP
       "Build the PostgreSQL-backed application entry point"
       OFF)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/eigen-5.0.0/Eigen")
    message(FATAL_ERROR
        "Eigen was not found. Place Eigen headers in eigen-5.0.0/ before configuring.")
endif()

add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/eigen-5.0.0"
)

add_library(portfolio_core STATIC
    constraint/ConstraintEngine.cpp
    constraint/portfolioConstraint.cpp
    constraint/tradeConstraint.cpp
    portfolio/PortfolioRepository.cpp
    portfolio/Security.cpp
    rebalancing/OptimizerEngine.cpp
    rebalancing/RebalanceEngine.cpp
    risk/metrics.cpp
    risk/returns.cpp
    risk/variance.cpp
)

target_include_directories(portfolio_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(portfolio_core PUBLIC eigen)

add_executable(rebalancing_tests
    rebalancing/test.cpp
)
target_link_libraries(rebalancing_tests PRIVATE portfolio_core)

enable_testing()
add_test(NAME rebalancing_tests COMMAND rebalancing_tests)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/osqp/CMakeLists.txt")
    set(OSQP_BUILD_SHARED_LIB OFF CACHE BOOL "" FORCE)
    set(OSQP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
    set(OSQP_BUILD_DEMO_EXE OFF CACHE BOOL "" FORCE)
    set(OSQP_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
    set(OSQP_ENABLE_PRINTING OFF CACHE BOOL "" FORCE)
    set(OSQP_ENABLE_PROFILING OFF CACHE BOOL "" FORCE)
    set(OSQP_ENABLE_INTERRUPT OFF CACHE BOOL "" FORCE)

    add_subdirectory(osqp EXCLUDE_FROM_ALL)

    add_executable(osqp_optimizer_tests
        test/optimizer.cpp
        test/prepoptimizer.cpp
        test/test.cpp
    )
    target_link_libraries(osqp_optimizer_tests PRIVATE osqpstatic)
    add_test(NAME osqp_optimizer_tests COMMAND osqp_optimizer_tests)
else()
    message(STATUS
        "OSQP was not found; osqp_optimizer_tests will not be built.")
endif()

if(PORTFOLIO_BUILD_DATABASE_APP)
    find_package(libpqxx CONFIG REQUIRED)

    add_executable(portfolio_app
        Database.cpp
        main.cpp
    )
    target_link_libraries(portfolio_app PRIVATE portfolio_core pqxx)
endif()
