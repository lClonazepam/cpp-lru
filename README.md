# cpp-lru

Header-only C++20 LRU cache with optional per-key TTL. O(1) get/put via list + hash map.

```cpp
kit::LruCache<std::string, int> cache(128);
cache.put("sku", 9, std::chrono::seconds(30));
auto v = cache.get("sku");
```

MIT
