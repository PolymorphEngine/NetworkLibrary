cmake_minimum_required(VERSION 3.17)
project(NetworkLibrary)

set(CMAKE_CXX_STANDARD 20)

if (NOT TARGET PolymorphNetwork)
    if (NOT TARGET asio)
        include(cmake/FindAsio.cmake)
        FIND_PACKAGE(asio REQUIRED)
    endif ()


    file(GLOB_RECURSE SRCS src/*.cpp)
    list(REMOVE_ITEM SRCS src/main.cpp)

    file(GLOB_RECURSE ICLS include/*.hpp)
    file(GLOB_RECURSE PRIV_ICLS src/include/*.hpp)

    get_target_property(asio-icls asio INTERFACE_INCLUDE_DIRECTORIES)

    add_library(PolymorphNetwork ${SRCS} ${ICLS} ${PRIV_ICLS})
    target_include_directories(PolymorphNetwork PUBLIC include)
    target_include_directories(PolymorphNetwork PRIVATE src/include ${asio-icls})
    set_target_properties(PolymorphNetwork PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(PolymorphNetwork asio)
    if(WIN32)
        # MSVC
        # /wd4100: unreferenced formal parameter (= unused parameter)
        target_compile_options(PolymorphNetwork PRIVATE /W4)
    endif()
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    include(tests/cmake/FindGoogleTest.cmake)
    enable_testing()

    add_subdirectory(tests)
endif ()
