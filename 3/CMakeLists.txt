cmake_minimum_required(VERSION 3.16)
project(ThreadPoolBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(benchmark REQUIRED)

add_library(thread_pool STATIC
    thread_pool.cpp
    thread_pool.h
)
target_include_directories(thread_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(histogram STATIC
    histogram.cpp
    histogram.h
)
target_link_libraries(histogram PRIVATE thread_pool)
target_include_directories(histogram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bench benchmark.cpp)
target_link_libraries(bench PRIVATE histogram thread_pool benchmark::benchmark pthread)
target_include_directories(bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(bench PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
