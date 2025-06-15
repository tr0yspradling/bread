#pragma once

#include <mutex>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

class cache_storage {
 public:
  void set(std::string key, std::string value);

  [[nodiscard]] std::optional<std::string> get(const std::string& key) const;

  [[nodiscard]] bool del(const std::string& key);

 private:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::string> store_;
};
