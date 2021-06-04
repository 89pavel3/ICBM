project(ICBM)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)
# target_include_directories(ICBM PUBLIC ${TEXTURES})


