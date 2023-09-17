cmake_minimum_required(VERSION 3.25)
project(stable_reinforcement_filter LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE "Release")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(MyProjectOptions)

add_subdirectory ("habf")

add_subdirectory("util")
