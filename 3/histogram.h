#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

constexpr size_t HISTOGRAM_SIZE = 256;

using Histogram = std::array<uint32_t, HISTOGRAM_SIZE>;

void histogram_naive(const uint8_t* data, size_t size, Histogram& hist);

void histogram_parallel(const uint8_t* data, size_t size, Histogram& hist, size_t num_threads);
