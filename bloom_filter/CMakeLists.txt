cmake_minimum_required(VERSION 3.25)
project(cache_backed_bloom_filter LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(MyProjectOptions)
add_subdirectory("bloom_filter")
add_subdirectory("hash_table")
add_subdirectory("main")
add_subdirectory("utils")
add_subdirectory("cache_backed_bloom_filter")
