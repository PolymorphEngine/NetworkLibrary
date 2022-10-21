cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE UNIT_TESTS_SRCS unit-tests/*.cpp)

add_executable(unit-tests ${UNIT_TESTS_SRCS})
target_link_libraries(unit-tests PRIVATE PolymorphNetwork GTest::gtest_main)
target_include_directories(unit-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/include)

include(GoogleTest)
gtest_discover_tests(unit-tests)