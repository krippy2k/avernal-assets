#include <avernal/assets/asset_manager.hpp>
#include <avernal/core/assert.hpp>
#include <algorithm>

namespace avernal {

AssetManager::AssetManager() = default;

AssetManager::~AssetManager() {
    clear_cache();
}

void AssetManager::register_loader(std::unique_ptr<IAssetLoader> loader) {
    AV_ASSERT(loader != nullptr);
    std::lock_guard lock(mutex_);
    loaders_[loader->type()] = std::move(loader);
}

void AssetManager::preload(AssetId id) {
    // Load the asset but don't return a handle
    // This keeps it in the cache
    std::lock_guard lock(mutex_);
    
    auto metadata = registry_.get_metadata(id);
    if (!metadata) {
        return;
    }
    
    auto* loader = find_loader(metadata->type);
    if (!loader) {
        return;
    }
    
    auto asset = loader->load(id, metadata->path, *metadata);
    if (asset) {
        asset->id_ = id;
        asset->path_ = metadata->path;
        loaded_assets_[id] = LoadedAsset{asset, *metadata};
    }
}

void AssetManager::preload(std::string_view path) {
    preload(AssetId{path});
}

void AssetManager::unload(AssetId id) {
    std::lock_guard lock(mutex_);
    loaded_assets_.erase(id);
}

bool AssetManager::reload(AssetId id) {
    std::lock_guard lock(mutex_);
    
    auto it = loaded_assets_.find(id);
    if (it == loaded_assets_.end()) {
        return false;
    }
    
    auto asset = it->second.asset.lock();
    if (!asset) {
        return false;
    }
    
    auto* loader = find_loader(it->second.metadata.type);
    if (!loader) {
        return false;
    }
    
    return loader->reload(asset.get());
}

bool AssetManager::is_loaded(AssetId id) const {
    std::lock_guard lock(mutex_);
    auto it = loaded_assets_.find(id);
    return it != loaded_assets_.end() && !it->second.asset.expired();
}

std::size_t AssetManager::loaded_count() const {
    std::lock_guard lock(mutex_);
    std::size_t count = 0;
    for (const auto& [id, loaded] : loaded_assets_) {
        if (!loaded.asset.expired()) {
            ++count;
        }
    }
    return count;
}

void AssetManager::clear_cache() {
    std::lock_guard lock(mutex_);
    loaded_assets_.clear();
}

void AssetManager::garbage_collect() {
    std::lock_guard lock(mutex_);
    
    // Remove assets with no active handles
    for (auto it = loaded_assets_.begin(); it != loaded_assets_.end();) {
        if (it->second.asset.expired()) {
            it = loaded_assets_.erase(it);
        } else {
            ++it;
        }
    }
}

IAssetLoader* AssetManager::find_loader(AssetType type) {
    auto it = loaders_.find(type);
    return it != loaders_.end() ? it->second.get() : nullptr;
}

IAssetLoader* AssetManager::find_loader_for_path(std::string_view path) {
    // Extract extension
    auto dot_pos = path.find_last_of('.');
    if (dot_pos == std::string_view::npos) {
        return nullptr;
    }
    
    std::string extension{path.substr(dot_pos)};
    
    // Find a loader that supports this extension
    for (auto& [type, loader] : loaders_) {
        auto extensions = loader->extensions();
        if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
            return loader.get();
        }
    }
    
    return nullptr;
}

} // namespace avernal
