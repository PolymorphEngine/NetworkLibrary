cmake_minimum_required(VERSION 3.17)
project(NetworkLibrary)

set(CMAKE_CXX_STANDARD 20)

set(REPOSITORY repository_dir)

if (NOT TARGET asio)
    include(cmake/FindAsio.cmake)
    FIND_PACKAGE(asio REQUIRED)
endif ()


file(GLOB_RECURSE SRCS src/*.cpp)
list(REMOVE_ITEM SRCS src/main.cpp)

file(GLOB_RECURSE ICLS include/*.hpp)
file(GLOB_RECURSE PRIV_ICLS src/include/*.hpp)

add_library(PolymorphNetwork SHARED ${SRCS} ${ICLS} ${PRIV_ICLS})

target_include_directories(PolymorphNetwork PUBLIC
        include
)

target_include_directories(PolymorphNetwork PRIVATE
        src/include
)

target_link_libraries(PolymorphNetwork
        asio
)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    if (NOT TARGET GTest::gtest_main)
        include(FetchContent)
        FetchContent_Declare(
                googletest
                GIT_REPOSITORY https://github.com/google/googletest.git
                GIT_TAG release-1.12.1
        )

        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif ()

    enable_testing()

    add_subdirectory(tests)


endif ()