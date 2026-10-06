#include "lru.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  kit::LruCache<int, std::string> cache(2);
  cache.put(1, "a");
  cache.put(2, "b");
  cache.put(3, "c");
  assert(!cache.get(1));
  assert(cache.get(2).value() == "b");
  cache.put(4, "d");
  assert(!cache.get(3));
  assert(cache.get(2).value() == "b");
  kit::LruCache<int, int> ttl(4);
  ttl.put(7, 1, std::chrono::milliseconds(30));
  assert(ttl.get(7).value() == 1);
  std::this_thread::sleep_for(std::chrono::milliseconds(40));
  assert(!ttl.get(7));
  std::cout << "lru ok\n";
}
