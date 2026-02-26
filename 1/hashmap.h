#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <optional>
#include <string>

typedef std::string TVal;
typedef int TKey;
typedef std::function<int(TKey, int)> THashFunction;

struct LinkedList {
  LinkedList *next;
  TKey key;
  TVal value;
};

class HashMap {
private:
  LinkedList **buckets;
  int bucketsSize;
  int size;
  THashFunction hashFunction;

public:
  HashMap(int capacity);

  HashMap(int capacity, THashFunction hf);

  ~HashMap();

  std::optional<TVal> remove(TKey key);

  bool has(TKey key);

  void set(TKey key, TVal val);

  int getSize();

  std::optional<TVal> get(TKey key);
};
