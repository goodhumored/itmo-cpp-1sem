#include "hashmap.h"
#include <cassert>
#include <functional>

THashFunction getDefaultHashFunction() {
  return [](TKey key, int capacity) -> int {
    return (key * 2654435761) % (1 << 30) % capacity;
  };
}

HashMap::HashMap(int capacity)
    : buckets(new LinkedList *[capacity]), bucketsSize(capacity), size(0),
      hashFunction(getDefaultHashFunction()) {
  std::fill(buckets, buckets + capacity, nullptr);
}

HashMap::HashMap(int capacity, THashFunction hf)
    : buckets(new LinkedList *[capacity]), bucketsSize(capacity), size(0),
      hashFunction(hf) {
  std::fill(buckets, buckets + capacity, nullptr);
}

HashMap::~HashMap() {
  for (int i = 0; i < bucketsSize; i++) {
    auto node = buckets[i];
    while (node) {
      auto next = node->next;
      delete node;
      node = next;
    }
  }
  delete[] buckets;
}

std::optional<TVal> HashMap::remove(TKey key) {
  int hash = hashFunction(key, bucketsSize);
  auto curr = buckets[hash];
  if (!curr)
    return std::nullopt;
  LinkedList *prev = nullptr;
  while (curr && curr->key != key) {
    prev = curr;
    curr = curr->next;
  }
  if (!curr)
    return std::nullopt;
  if (prev)
    prev->next = curr->next;
  else
    buckets[hash] = curr->next;
  TVal value = curr->value;
  delete curr;
  size--;
  return value;
}

bool HashMap::has(TKey key) {
  auto bucket = buckets[hashFunction(key, bucketsSize)];
  if (!bucket)
    return false;
  while (bucket->next && bucket->key != key)
    bucket = bucket->next;
  return bucket->key == key;
}

void HashMap::set(TKey key, TVal val) {
  int hash = hashFunction(key, bucketsSize);
  auto bucket = buckets[hash];
  if (!bucket) {
    buckets[hash] = new LinkedList{nullptr, key, val};
    size++;
    return;
  }
  while (bucket->next && bucket->key != key)
    bucket = bucket->next;
  if (bucket->key == key)
    bucket->value = val;
  else {
    bucket->next = new LinkedList{nullptr, key, val};
    size++;
  }
}

int HashMap::getSize() { return size; }

std::optional<TVal> HashMap::get(TKey key) {
  auto bucket = buckets[hashFunction(key, bucketsSize)];
  if (!bucket)
    return std::nullopt;
  while (bucket->next && bucket->key != key)
    bucket = bucket->next;
  if (bucket->key == key)
    return bucket->value;
  return std::nullopt;
}
