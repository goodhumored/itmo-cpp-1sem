#include "histogram.h"
#include "thread_pool.h"
#include <cstring>
#include <vector>

void histogram_naive(const uint8_t* data, size_t size, Histogram& hist) {
    std::memset(hist.data(), 0, sizeof(Histogram));
    for (size_t i = 0; i < size; ++i) {
        ++hist[data[i]];
    }
}

void histogram_parallel(const uint8_t* data, size_t size, Histogram& hist, size_t num_threads) {
    std::memset(hist.data(), 0, sizeof(Histogram));
    
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }
    
    ThreadPool pool(num_threads);
    
    std::vector<Histogram> local_hists(num_threads);
    std::vector<std::future<void>> futures;
    futures.reserve(num_threads);
    
    size_t chunk_size = size / num_threads;
    size_t remainder = size % num_threads;
    
    size_t offset = 0;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        
        if (current_chunk == 0) continue;
        
        const uint8_t* chunk_data = data + offset;
        Histogram& local_hist = local_hists[i];
        
        futures.push_back(pool.submit([chunk_data, current_chunk, &local_hist]() {
            std::memset(local_hist.data(), 0, sizeof(Histogram));
            for (size_t j = 0; j < current_chunk; ++j) {
                ++local_hist[chunk_data[j]];
            }
        }));
        
        offset += current_chunk;
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    for (const auto& local_hist : local_hists) {
        for (size_t i = 0; i < HISTOGRAM_SIZE; ++i) {
            hist[i] += local_hist[i];
        }
    }
}
