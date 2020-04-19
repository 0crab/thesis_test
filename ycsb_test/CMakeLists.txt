cmake_minimum_required(VERSION 3.13)
project(thesis_test)

set(CMAKE_CXX_STANDARD 14)


add_executable(ycsb ycsb.cpp )

target_link_libraries(ycsb pthread)