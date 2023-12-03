cmake_minimum_required(VERSION 3.24)
project(mazeSolver)

set(CMAKE_CXX_STANDARD 17)

add_executable(mazeSolver main.cpp)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(mazeSolver sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
