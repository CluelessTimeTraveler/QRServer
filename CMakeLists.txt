cmake_minimum_required(VERSION 3.15)
project(QRServer)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads)
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(QRServer main.cpp)