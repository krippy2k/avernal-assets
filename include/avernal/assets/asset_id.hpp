#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <functional>

namespace avernal {

/// Unique identifier for an asset
/// Can be created from a path string or a numeric ID
class AssetId {
public:
    using IdType = std::uint64_t;

    constexpr AssetId() noexcept = default;
    constexpr explicit AssetId(IdType id) noexcept : id_(id) {}
    
    /// Create an AssetId from a path string (uses hash)
    explicit AssetId(std::string_view path) noexcept;
    
    /// Get the numeric ID
    [[nodiscard]] constexpr IdType id() const noexcept { return id_; }
    
    /// Check if this is a valid asset ID
    [[nodiscard]] constexpr bool is_valid() const noexcept { return id_ != 0; }
    
    /// Comparison operators
    [[nodiscard]] constexpr bool operator==(const AssetId& other) const noexcept {
        return id_ == other.id_;
    }
    
    [[nodiscard]] constexpr bool operator!=(const AssetId& other) const noexcept {
        return id_ != other.id_;
    }
    
    [[nodiscard]] constexpr bool operator<(const AssetId& other) const noexcept {
        return id_ < other.id_;
    }

private:
    IdType id_{0};
};

} // namespace avernal

// Hash support for std::unordered_map
template<>
struct std::hash<avernal::AssetId> {
    std::size_t operator()(const avernal::AssetId& id) const noexcept {
        return std::hash<avernal::AssetId::IdType>{}(id.id());
    }
};
