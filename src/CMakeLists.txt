add_executable(ICBM_test test.cpp ICBM.cpp)
target_link_libraries(ICBM_test PUBLIC doctest::doctest default)
doctest_discover_tests(ICBM_test)

add_executable(ICBM ICBM.h ICBM.cpp)
target_link_libraries(ICBM ${CONAN_LIBS})
