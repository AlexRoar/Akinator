cmake_minimum_required(VERSION 3.17)
project(Akinator CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(CppSwiftyList)
include_directories(CppSwiftyList/${SL_SOURCE_DIR})
include_directories(Akinator)

add_library(AkinatorLib STATIC Akinator/Algo/StringDistance.cpp)

add_executable(AkinatorExample Examples/BasicExample.cpp Akinator/Akinator.h)
target_link_libraries(AkinatorExample AkinatorLib)