cmake_minimum_required(VERSION 3.25)
project(connect4montecarlo VERSION 1.0 LANGUAGES CXX)

add_library(game OBJECT game.cpp)

add_executable(connect4montecarlo connect4montecarlo.cc)
target_link_libraries(connect4montecarlo game)

find_package(GTest REQUIRED)
add_executable(connect4montecarlo-test connect4montecarlo-test.cc)
target_link_libraries(connect4montecarlo-test GTest::gtest_main game)
enable_testing()
gtest_discover_tests(connect4montecarlo-test)
