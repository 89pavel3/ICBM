project(idk)
cmake_minimum_required(VERSION 2.8.12)
add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(idk idk.cpp)
target_link_libraries(idk ${CONAN_LIBS})
