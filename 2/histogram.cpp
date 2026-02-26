#include "histogram.h"
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #define USE_AVX2 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #include <arm_neon.h>
    #define USE_NEON 1
#endif

void histogram_naive(const uint8_t* data, size_t size, Histogram& hist) {
    std::memset(hist.data(), 0, sizeof(Histogram));
    for (size_t i = 0; i < size; ++i) {
        ++hist[data[i]];
    }
}

#if defined(USE_NEON)

void histogram_simd(const uint8_t* data, size_t size, Histogram& hist) {
    std::memset(hist.data(), 0, sizeof(Histogram));
    
    alignas(64) uint32_t local_hist[4][HISTOGRAM_SIZE] = {};
    
    size_t i = 0;
    const size_t simd_end = size - (size % 16);
    
    for (; i < simd_end; i += 16) {
        uint8x16_t pixels = vld1q_u8(data + i);
        
        ++local_hist[0][vgetq_lane_u8(pixels, 0)];
        ++local_hist[1][vgetq_lane_u8(pixels, 1)];
        ++local_hist[2][vgetq_lane_u8(pixels, 2)];
        ++local_hist[3][vgetq_lane_u8(pixels, 3)];
        ++local_hist[0][vgetq_lane_u8(pixels, 4)];
        ++local_hist[1][vgetq_lane_u8(pixels, 5)];
        ++local_hist[2][vgetq_lane_u8(pixels, 6)];
        ++local_hist[3][vgetq_lane_u8(pixels, 7)];
        ++local_hist[0][vgetq_lane_u8(pixels, 8)];
        ++local_hist[1][vgetq_lane_u8(pixels, 9)];
        ++local_hist[2][vgetq_lane_u8(pixels, 10)];
        ++local_hist[3][vgetq_lane_u8(pixels, 11)];
        ++local_hist[0][vgetq_lane_u8(pixels, 12)];
        ++local_hist[1][vgetq_lane_u8(pixels, 13)];
        ++local_hist[2][vgetq_lane_u8(pixels, 14)];
        ++local_hist[3][vgetq_lane_u8(pixels, 15)];
    }
    
    for (; i < size; ++i) {
        ++local_hist[0][data[i]];
    }
    
    for (size_t j = 0; j < HISTOGRAM_SIZE; j += 4) {
        uint32x4_t sum0 = vld1q_u32(&local_hist[0][j]);
        uint32x4_t sum1 = vld1q_u32(&local_hist[1][j]);
        uint32x4_t sum2 = vld1q_u32(&local_hist[2][j]);
        uint32x4_t sum3 = vld1q_u32(&local_hist[3][j]);
        
        uint32x4_t total = vaddq_u32(vaddq_u32(sum0, sum1), vaddq_u32(sum2, sum3));
        vst1q_u32(&hist[j], total);
    }
}

#elif defined(USE_AVX2)

void histogram_simd(const uint8_t* data, size_t size, Histogram& hist) {
    std::memset(hist.data(), 0, sizeof(Histogram));
    
    alignas(64) uint32_t local_hist[4][HISTOGRAM_SIZE] = {};
    
    size_t i = 0;
    const size_t simd_end = size - (size % 32);
    
    for (; i < simd_end; i += 32) {
        __m256i pixels = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        
        alignas(32) uint8_t buffer[32];
        _mm256_store_si256(reinterpret_cast<__m256i*>(buffer), pixels);
        
        for (int j = 0; j < 32; j += 4) {
            ++local_hist[0][buffer[j]];
            ++local_hist[1][buffer[j + 1]];
            ++local_hist[2][buffer[j + 2]];
            ++local_hist[3][buffer[j + 3]];
        }
    }
    
    for (; i < size; ++i) {
        ++local_hist[0][data[i]];
    }
    
    for (size_t j = 0; j < HISTOGRAM_SIZE; j += 8) {
        __m256i sum0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&local_hist[0][j]));
        __m256i sum1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&local_hist[1][j]));
        __m256i sum2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&local_hist[2][j]));
        __m256i sum3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&local_hist[3][j]));
        
        __m256i total = _mm256_add_epi32(_mm256_add_epi32(sum0, sum1), _mm256_add_epi32(sum2, sum3));
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(&hist[j]), total);
    }
}

#else

void histogram_simd(const uint8_t* data, size_t size, Histogram& hist) {
    histogram_naive(data, size, hist);
}

#endif
