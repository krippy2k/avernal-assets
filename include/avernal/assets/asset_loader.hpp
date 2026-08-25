#pragma once

#include <avernal/assets/asset_handle.hpp>
#include <avernal/assets/asset_metadata.hpp>
#include <memory>
#include <string_view>
#include <vector>

namespace avernal {

/// Interface for loading specific asset types
/// Implement this for each asset type (textures, meshes, etc.)
class IAssetLoader {
public:
    virtual ~IAssetLoader() = default;
    
    /// Get the asset type this loader handles
    [[nodiscard]] virtual AssetType type() const noexcept = 0;
    
    /// Get file extensions this loader supports (e.g., ".png", ".jpg")
    [[nodiscard]] virtual std::vector<std::string> extensions() const = 0;
    
    /// Load an asset from a file
    [[nodiscard]] virtual std::shared_ptr<Asset> load(
        AssetId id,
        std::string_view path,
        const AssetMetadata& metadata
    ) = 0;
    
    /// Unload an asset (optional cleanup)
    virtual void unload(Asset* asset) {}
    
    /// Reload an asset (useful for hot-reloading)
    virtual bool reload(Asset* asset) { return false; }
};

} // namespace avernal
