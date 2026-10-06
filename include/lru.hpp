#pragma once
#include <chrono>
#include <cstddef>
#include <list>
#include <optional>
#include <unordered_map>
#include <utility>

namespace kit {

template <typename Key, typename Value>
class LruCache {
 public:
  using Clock = std::chrono::steady_clock;
  explicit LruCache(std::size_t capacity) : capacity_(capacity == 0 ? 1 : capacity) {}

  void put(Key key, Value value, Clock::duration ttl = Clock::duration::zero()) {
    const auto now = Clock::now();
    auto it = index_.find(key);
    if (it != index_.end()) {
      it->second->value = std::move(value);
      it->second->expires = expiry(now, ttl);
      touch(it->second);
      return;
    }
    order_.push_front(Entry{std::move(key), std::move(value), expiry(now, ttl)});
    index_.emplace(order_.front().key, order_.begin());
    evict(now);
  }

  std::optional<Value> get(const Key& key) {
    const auto now = Clock::now();
    auto it = index_.find(key);
    if (it == index_.end()) return std::nullopt;
    if (expired(it->second->expires, now)) {
      order_.erase(it->second);
      index_.erase(it);
      return std::nullopt;
    }
    touch(it->second);
    return it->second->value;
  }

  bool contains(const Key& key) { return get(key).has_value(); }
  std::size_t size() const { return index_.size(); }
  std::size_t capacity() const { return capacity_; }

 private:
  struct Entry {
    Key key;
    Value value;
    Clock::time_point expires;
  };
  using List = std::list<Entry>;
  static Clock::time_point expiry(Clock::time_point now, Clock::duration ttl) {
    if (ttl <= Clock::duration::zero()) return Clock::time_point::max();
    return now + ttl;
  }
  static bool expired(Clock::time_point expires, Clock::time_point now) { return expires <= now; }
  void touch(typename List::iterator it) { order_.splice(order_.begin(), order_, it); }
  void evict(Clock::time_point now) {
    while (!order_.empty() && expired(order_.back().expires, now)) {
      index_.erase(order_.back().key);
      order_.pop_back();
    }
    while (index_.size() > capacity_) {
      index_.erase(order_.back().key);
      order_.pop_back();
    }
  }
  std::size_t capacity_;
  List order_;
  std::unordered_map<Key, typename List::iterator> index_;
};

}  // namespace kit
