#pragma once

#include <bread/storage_engine.h>

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

namespace bread {

/// Storage engine that adds per-key expiration (TTL) semantics on top of
/// another engine.
class expiration_engine : public storage_engine {
 public:
  explicit expiration_engine(std::unique_ptr<storage_engine> inner)
      : inner_(std::move(inner)) {}

  void set(std::string key, std::string value, int exptime = 0) override;
  std::optional<std::string> get(const std::string& key) const override;
  bool del(const std::string& key) override;

 private:
  std::unique_ptr<storage_engine> inner_;
  mutable std::mutex mutex_;
  mutable std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      expirations_;
};

}  // namespace bread