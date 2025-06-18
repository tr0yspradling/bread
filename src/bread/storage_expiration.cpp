#include <bread/storage_expiration.h>

using namespace std::chrono;

void bread::expiration_engine::set(std::string key, std::string value,
                                   int exptime) {
  auto now = steady_clock::now();
  {
    std::lock_guard lock(mutex_);
    if (exptime > 0) {
      expirations_[key] = now + seconds(exptime);
    } else {
      expirations_.erase(key);
    }
  }
  inner_->set(std::move(key), std::move(value), exptime);
}

std::optional<std::string> bread::expiration_engine::get(
    const std::string& key) const {
  auto now = steady_clock::now();
  {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = expirations_.find(key);
    if (it != expirations_.end() && now >= it->second) {
      expirations_.erase(it);
      inner_->del(key);
      return std::nullopt;
    }
  }
  return inner_->get(key);
}

bool bread::expiration_engine::del(const std::string& key) {
  std::lock_guard lock(mutex_);
  expirations_.erase(key);
  return inner_->del(key);
}