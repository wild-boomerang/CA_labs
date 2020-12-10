cmake_minimum_required(VERSION 3.16)
project(Lab_3)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_3 main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")