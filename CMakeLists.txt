cmake_minimum_required(VERSION 2.8.12)

enable_testing()

cmake_policy(SET CMP0048 NEW)
project(Texas VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(test)
