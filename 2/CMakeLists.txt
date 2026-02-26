cmake_minimum_required(VERSION 3.16)
project(HistogramBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(benchmark REQUIRED)

add_library(histogram STATIC
    histogram.cpp
    histogram.h
)
target_include_directories(histogram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    target_compile_options(histogram PRIVATE -march=native)
else()
    target_compile_options(histogram PRIVATE -march=native -mavx2)
endif()

add_executable(bench benchmark.cpp)
target_link_libraries(bench PRIVATE histogram benchmark::benchmark pthread)
target_include_directories(bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    target_compile_options(bench PRIVATE -march=native)
else()
    target_compile_options(bench PRIVATE -march=native -mavx2)
endif()

set_target_properties(bench PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
