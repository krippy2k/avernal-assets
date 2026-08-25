#pragma once

#include <avernal/assets/asset_handle.hpp>
#include <avernal/assets/asset_registry.hpp>
#include <avernal/assets/asset_loader.hpp>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <string_view>

namespace avernal {

/// Central manager for loading, caching, and unloading assets
/// Thread-safe asset loading and access
class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    
    /// Register an asset loader for a specific type
    void register_loader(std::unique_ptr<IAssetLoader> loader);
    
    /// Get the asset registry
    [[nodiscard]] AssetRegistry& registry() noexcept { return registry_; }
    [[nodiscard]] const AssetRegistry& registry() const noexcept { return registry_; }
    
    /// Load an asset by ID
    template<typename T>
    [[nodiscard]] AssetHandle<T> load(AssetId id);
    
    /// Load an asset by path
    template<typename T>
    [[nodiscard]] AssetHandle<T> load(std::string_view path);
    
    /// Preload an asset (load into cache but don't return handle)
    void preload(AssetId id);
    void preload(std::string_view path);
    
    /// Unload an asset from cache (will be destroyed when all handles are released)
    void unload(AssetId id);
    
    /// Reload an asset (useful for hot-reloading during development)
    bool reload(AssetId id);
    
    /// Check if an asset is loaded in memory
    [[nodiscard]] bool is_loaded(AssetId id) const;
    
    /// Get number of loaded assets
    [[nodiscard]] std::size_t loaded_count() const;
    
    /// Clear all cached assets (will unload when all handles are released)
    void clear_cache();
    
    /// Garbage collect assets with no active handles
    void garbage_collect();

private:
    struct LoadedAsset {
        std::weak_ptr<Asset> asset;
        AssetMetadata metadata;
    };
    
    [[nodiscard]] IAssetLoader* find_loader(AssetType type);
    [[nodiscard]] IAssetLoader* find_loader_for_path(std::string_view path);
    
    AssetRegistry registry_;
    std::unordered_map<AssetType, std::unique_ptr<IAssetLoader>> loaders_;
    std::unordered_map<AssetId, LoadedAsset> loaded_assets_;
    
    mutable std::mutex mutex_;
};

// Template implementations

template<typename T>
AssetHandle<T> AssetManager::load(AssetId id) {
    std::lock_guard lock(mutex_);
    
    // Check if already loaded
    auto it = loaded_assets_.find(id);
    if (it != loaded_assets_.end()) {
        if (auto asset = it->second.asset.lock()) {
            return AssetHandle<T>(std::static_pointer_cast<T>(asset));
        }
    }
    
    // Get metadata
    auto metadata = registry_.get_metadata(id);
    if (!metadata) {
        return AssetHandle<T>{};
    }
    
    // Find appropriate loader
    auto* loader = find_loader(metadata->type);
    if (!loader) {
        return AssetHandle<T>{};
    }
    
    // Load the asset
    auto asset = loader->load(id, metadata->path, *metadata);
    if (!asset) {
        return AssetHandle<T>{};
    }
    
    asset->id_ = id;
    asset->path_ = metadata->path;
    
    // Cache it
    loaded_assets_[id] = LoadedAsset{asset, *metadata};
    
    return AssetHandle<T>(std::static_pointer_cast<T>(asset));
}

template<typename T>
AssetHandle<T> AssetManager::load(std::string_view path) {
    AssetId id{path};
    std::lock_guard lock(mutex_);
    
    // Check if already loaded
    auto it = loaded_assets_.find(id);
    if (it != loaded_assets_.end()) {
        if (auto asset = it->second.asset.lock()) {
            return AssetHandle<T>(std::static_pointer_cast<T>(asset));
        }
    }
    
    // Try to get metadata from registry first
    auto metadata = registry_.get_metadata(id);
    IAssetLoader* loader = nullptr;
    
    if (metadata) {
        // Use metadata-based loader lookup
        loader = find_loader(metadata->type);
    } else {
        // Fall back to extension-based loader lookup for direct path loading
        loader = find_loader_for_path(path);
        
        // Create minimal metadata
        if (loader) {
            metadata = AssetMetadata{
                .id = id,
                .path = std::string{path},
                .type = loader->type(),
            };
        }
    }
    
    if (!loader || !metadata) {
        return AssetHandle<T>{};
    }
    
    // Load the asset
    auto asset = loader->load(id, metadata->path, *metadata);
    if (!asset) {
        return AssetHandle<T>{};
    }
    
    asset->id_ = id;
    asset->path_ = metadata->path;
    
    // Cache it
    loaded_assets_[id] = LoadedAsset{asset, *metadata};
    
    return AssetHandle<T>(std::static_pointer_cast<T>(asset));
}

} // namespace avernal
