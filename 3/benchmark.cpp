#include "histogram.h"
#include "thread_pool.h"
#include <benchmark/benchmark.h>
#include <vector>
#include <random>

std::vector<uint8_t> generate_random_image(size_t size) {
    std::vector<uint8_t> data(size);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, 255);
    for (auto& pixel : data) {
        pixel = static_cast<uint8_t>(dist(gen));
    }
    return data;
}

static void BM_Histogram_Naive(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_naive(data.data(), data.size(), hist);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Naive)->RangeMultiplier(4)->Range(1 << 16, 1 << 26);

static void BM_Histogram_Parallel_2(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_parallel(data.data(), data.size(), hist, 2);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Parallel_2)->RangeMultiplier(4)->Range(1 << 16, 1 << 26);

static void BM_Histogram_Parallel_4(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_parallel(data.data(), data.size(), hist, 4);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Parallel_4)->RangeMultiplier(4)->Range(1 << 16, 1 << 26);

static void BM_Histogram_Parallel_8(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_parallel(data.data(), data.size(), hist, 8);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Parallel_8)->RangeMultiplier(4)->Range(1 << 16, 1 << 26);

static void BM_Histogram_Parallel_Auto(benchmark::State& state) {
    auto data = generate_random_image(state.range(0));
    Histogram hist;
    
    for (auto _ : state) {
        histogram_parallel(data.data(), data.size(), hist, 0);
        benchmark::DoNotOptimize(hist);
    }
    
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(state.range(0)));
}
BENCHMARK(BM_Histogram_Parallel_Auto)->RangeMultiplier(4)->Range(1 << 16, 1 << 26);

static void BM_ThreadPool_Overhead(benchmark::State& state) {
    ThreadPool pool(4);
    
    for (auto _ : state) {
        auto future = pool.submit([]() { return 42; });
        benchmark::DoNotOptimize(future.get());
    }
}
BENCHMARK(BM_ThreadPool_Overhead);

static void BM_ThreadPool_Batch(benchmark::State& state) {
    ThreadPool pool(4);
    const int batch_size = state.range(0);
    
    for (auto _ : state) {
        std::vector<std::future<int>> futures;
        futures.reserve(batch_size);
        
        for (int i = 0; i < batch_size; ++i) {
            futures.push_back(pool.submit([i]() { return i * 2; }));
        }
        
        for (auto& f : futures) {
            benchmark::DoNotOptimize(f.get());
        }
    }
    
    state.SetItemsProcessed(int64_t(state.iterations()) * batch_size);
}
BENCHMARK(BM_ThreadPool_Batch)->RangeMultiplier(4)->Range(16, 4096);

BENCHMARK_MAIN();
