#pragma once

#include <avernal/assets/asset_id.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace avernal {

/// Type of asset
enum class AssetType {
    unknown,
    texture,
    mesh,
    material,
    shader,
    audio,
    scene,
    prefab,
    model,
};

/// Metadata associated with an asset
struct AssetMetadata {
    AssetId id;
    std::string path;
    AssetType type{AssetType::unknown};
    
    /// Dependencies (other assets this asset depends on)
    std::vector<AssetId> dependencies;
    
    /// File information
    std::uint64_t file_size{0};
    std::chrono::system_clock::time_point last_modified;
    
    /// Custom metadata (key-value pairs)
    std::unordered_map<std::string, std::string> custom;
    
    /// Check if the asset file has been modified since metadata was created
    [[nodiscard]] bool is_outdated() const;
};

/// Convert AssetType to string
[[nodiscard]] const char* to_string(AssetType type) noexcept;

/// Convert string to AssetType
[[nodiscard]] AssetType asset_type_from_extension(std::string_view extension) noexcept;

} // namespace avernal
