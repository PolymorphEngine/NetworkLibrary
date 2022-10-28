cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE UNIT_TESTS_SRCS unit-tests/*.cpp)
file(GLOB_RECURSE E2E_TESTS_SRCS e2e-tests/*.cpp)

add_executable(test-exec main.cpp)
target_link_libraries(test-exec PRIVATE PolymorphNetwork)
set_target_properties(test-exec PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(unit-tests ${UNIT_TESTS_SRCS})
target_link_libraries(unit-tests PRIVATE PolymorphNetwork GTest::gtest_main)
target_include_directories(unit-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/include)

add_executable(e2e-tests ${E2E_TESTS_SRCS})
target_link_libraries(e2e-tests PRIVATE PolymorphNetwork GTest::gtest_main)
target_include_directories(e2e-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/include)

include(GoogleTest)
gtest_discover_tests(unit-tests)
gtest_discover_tests(e2e-tests)