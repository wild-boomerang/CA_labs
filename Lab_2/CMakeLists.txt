cmake_minimum_required(VERSION 3.16)
project(Lab_2)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SRC
        "src/*.h"
        "src/*.cpp"
        )

add_executable(Lab_2 ${SRC})