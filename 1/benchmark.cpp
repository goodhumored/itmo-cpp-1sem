#include "hashmap.h"
#include <benchmark/benchmark.h>

constexpr size_t CAPACITY = 1 << 20;

int CACHE_NO_COLLISIONS(TKey key, int capacity) {
    return key % capacity;
}

int CACHE_MANY_COLLISIONS(TKey key, int capacity) {
    return (key / 100) % capacity;
}

int CACHE_RANDOM_COLLISIONS(TKey key, int capacity) {
    return (key * 2654435761) % (1 << 30) % capacity;
}

static void BM_HashMap_Set_No_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
  }
}
// BENCHMARK(BM_HashMap_Set_No_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY); // от 1024 до 1M элементов

static void BM_HashMap_Set_Many_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_MANY_COLLISIONS);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
  }
}
// BENCHMARK(BM_HashMap_Set_Many_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Set_Random_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_RANDOM_COLLISIONS);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
  }
}
// BENCHMARK(BM_HashMap_Set_Random_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Delete_No_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_NO_COLLISIONS);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
    for (int i = 0; i < state.range(0); i++) {
      map.remove(i);
    }
  }
}
// BENCHMARK(BM_HashMap_Delete_No_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Delete_Many_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_MANY_COLLISIONS);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
    for (int i = 0; i < state.range(0); i++) {
      map.remove(i);
    }
  }
}
// BENCHMARK(BM_HashMap_Delete_Many_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Delete_Random_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_RANDOM_COLLISIONS);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0) / 1.4; i++) {
      map.set(i, std::string(1000, 'a') + std::to_string(i));
    }
    for (int i = 0; i < state.range(0); i++) {
      map.remove(i);
    }
  }
}
// BENCHMARK(BM_HashMap_Delete_Random_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Get_No_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_NO_COLLISIONS);
  for (int i = 0; i < state.range(0); i++) {
    map.set(i, std::string(1000, 'a') + std::to_string(i));
  }
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      benchmark::DoNotOptimize(map.get(i));
    }
  }
}
BENCHMARK(BM_HashMap_Get_No_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Get_Many_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_MANY_COLLISIONS);
  for (int i = 0; i < state.range(0); i++) {
    map.set(i, std::string(1000, 'a') + std::to_string(i));
  }
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      benchmark::DoNotOptimize(map.get(i));
    }
  }
}
BENCHMARK(BM_HashMap_Get_Many_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

static void BM_HashMap_Get_Random_Collisions(benchmark::State &state) {
  HashMap map(CAPACITY, CACHE_RANDOM_COLLISIONS);
  for (int i = 0; i < state.range(0); i++) {
    map.set(i, std::string(1000, 'a') + std::to_string(i));
  }
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      benchmark::DoNotOptimize(map.get(i));
    }
  }
}
BENCHMARK(BM_HashMap_Get_Random_Collisions)->RangeMultiplier(2)->Range(1 << 10, CAPACITY);

BENCHMARK_MAIN(); // <-- генерирует main автоматически
