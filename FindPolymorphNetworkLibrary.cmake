cmake_minimum_required(VERSION 3.17)

if (NOT TARGET PolymorphNetwork)
    include(FetchContent)
    FetchContent_Declare(
        PolymorphNetwork
        GIT_REPOSITORY git@github.com:PolymorphEngine/NetworkLibrary.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(PolymorphNetwork)
endif()