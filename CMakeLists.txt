cmake_minimum_required(VERSION 3.5)

project(myhttpd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED system thread)

add_library(request SHARED
    request.h request.cpp)

add_library(response SHARED
    response.h response.cpp)

add_library(httpserver SHARED
    httpserver.h httpserver.cpp)

target_link_libraries(httpserver
    request
    response
    ${Boost_LIBRARIES})

add_executable(myhttpd main.cpp)

target_link_libraries(myhttpd
    httpserver)

add_executable(test
    test.h test.cpp)

target_link_libraries(test
    request
    response
    httpserver)

