#include "histogram.h"
#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

std::vector<uint8_t> generate_random_image(size_t size) {
    std::vector<uint8_t> data(size);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& pixel : data) {
        pixel = static_cast<uint8_t>(dist(gen));
    }
    return data;
}

std::vector<uint8_t> generate_uniform_image(size_t size, uint8_t value) {
    return std::vector<uint8_t>(size, value);
}

std::vector<uint8_t> generate_gradient_image(size_t size) {
    std::vector<uint8_t> data(size);
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<uint8_t>(i % 256);
    }
    return data;
}

static void BM_Histogram_Naive_Random(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_naive(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Naive_Random)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

static void BM_Histogram_SIMD_Random(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_simd(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_SIMD_Random)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

static void BM_Histogram_Naive_Uniform(benchmark::State& state) {
    auto data = generate_uniform_image(state.range(0), 128);
    Histogram hist;
    
    for (auto _ : state) {
        histogram_naive(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Naive_Uniform)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

static void BM_Histogram_SIMD_Uniform(benchmark::State& state) {
    auto data = generate_uniform_image(state.range(0), 128);
    Histogram hist;
    
    for (auto _ : state) {
        histogram_simd(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_SIMD_Uniform)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

static void BM_Histogram_Naive_Gradient(benchmark::State& state) {
    auto data = generate_gradient_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_naive(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Naive_Gradient)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

static void BM_Histogram_SIMD_Gradient(benchmark::State& state) {
    auto data = generate_gradient_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_simd(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_SIMD_Gradient)->RangeMultiplier(4)->Range(1 << 12, 1 << 24);

BENCHMARK_MAIN();
