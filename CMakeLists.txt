cmake_minimum_required(VERSION 3.10.0)
project(ICBM CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)
target_include_directories(ICBM PUBLIC resources)


