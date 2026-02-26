cmake_minimum_required(VERSION 3.16)
project(HashMapBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------
# Определяем путь к Google Benchmark
# ----------------------
find_package(benchmark REQUIRED)

# ----------------------
# Таргет с реализацией HashMap
# ----------------------
add_library(hashmap STATIC
    hashmap.cpp
    hashmap.h
)
target_include_directories(hashmap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ----------------------
# Таргет для бенчмарков
# ----------------------
add_executable(bench benchmark.cpp)
target_link_libraries(bench PRIVATE hashmap benchmark::benchmark pthread)
target_include_directories(bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ----------------------
# Настройки для Release (оптимизация)
# ----------------------
set_target_properties(bench PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
