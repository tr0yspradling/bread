// Abstract interface for pluggable storage engines.
#pragma once

#include <optional>
#include <string>

namespace bread {

/// Interface for storage backends (e.g., in-memory, LRU cache, disk-backed).
class storage_engine {
 public:
  virtual ~storage_engine() = default;

  /// Store a value for the given key.
  virtual void set(std::string key, std::string value) = 0;

  /// Retrieve the value for the given key, or std::nullopt if not found.
  virtual std::optional<std::string> get(const std::string& key) const = 0;

  /// Delete the given key. Returns true if the key was removed.
  virtual bool del(const std::string& key) = 0;
};

}  // namespace bread