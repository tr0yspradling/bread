#include <bread/storage.h>

#include <mutex>
#include <optional>
#include <shared_mutex>

void cache_storage::set(std::string key, std::string value) {
  std::unique_lock lock(mutex_);
  store_[std::move(key)] = std::move(value);
}

std::optional<std::string> cache_storage::get(const std::string& key) const {
  std::shared_lock lock(mutex_);
  if (auto it = store_.find(key); it != store_.end()) {
    return it->second;
  }
  return std::nullopt;
}

bool cache_storage::del(const std::string& key) {
  std::unique_lock lock(mutex_);
  return store_.erase(key) > 0;
}
