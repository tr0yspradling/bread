// In-memory storage implementation for bread cache server.
#pragma once

#include <bread/storage_engine.h>

#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace bread {

/// Default in-memory storage engine.
class cache_storage : public storage_engine {
 public:
  void set(std::string key, std::string value, int exptime = 0) override;

  [[nodiscard]] std::optional<std::string> get(
      const std::string& key) const override;

  [[nodiscard]] bool del(const std::string& key) override;

 private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::string> store_;
};

}  // namespace bread
