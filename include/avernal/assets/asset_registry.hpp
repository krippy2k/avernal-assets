#pragma once

#include <avernal/assets/asset_id.hpp>
#include <avernal/assets/asset_metadata.hpp>
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>

namespace avernal {

/// Registry of all known assets and their metadata
/// Stores information about assets without loading them into memory
class AssetRegistry {
public:
    AssetRegistry() = default;
    ~AssetRegistry() = default;
    
    AssetRegistry(const AssetRegistry&) = delete;
    AssetRegistry& operator=(const AssetRegistry&) = delete;
    
    /// Register an asset with its metadata
    void register_asset(const AssetMetadata& metadata);
    
    /// Unregister an asset
    void unregister_asset(AssetId id);
    
    /// Get metadata for an asset
    [[nodiscard]] std::optional<AssetMetadata> get_metadata(AssetId id) const;
    
    /// Check if an asset is registered
    [[nodiscard]] bool is_registered(AssetId id) const;
    
    /// Get all assets of a specific type
    [[nodiscard]] std::vector<AssetMetadata> get_assets_by_type(AssetType type) const;
    
    /// Get all registered assets
    [[nodiscard]] std::vector<AssetMetadata> get_all_assets() const;
    
    /// Scan a directory and register all assets
    void scan_directory(const std::string& path, bool recursive = true);
    
    /// Save registry to a file (asset catalog)
    bool save_to_file(const std::string& path) const;
    
    /// Load registry from a file
    bool load_from_file(const std::string& path);
    
    /// Clear all registered assets
    void clear();

private:
    std::unordered_map<AssetId, AssetMetadata> assets_;
    std::unordered_map<std::string, AssetId> path_to_id_;
};

} // namespace avernal
