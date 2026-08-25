#pragma once

#include <avernal/assets/asset_id.hpp>
#include <memory>
#include <atomic>

namespace avernal {

class AssetManager;

/// Base class for all assets
class Asset {
public:
    virtual ~Asset() = default;
    
    [[nodiscard]] AssetId id() const noexcept { return id_; }
    [[nodiscard]] const std::string& path() const noexcept { return path_; }
    
protected:
    friend class AssetManager;
    AssetId id_;
    std::string path_;
};

/// Reference-counted handle to an asset
/// Automatically loads and unloads assets based on reference count
template<typename T>
class AssetHandle {
public:
    AssetHandle() noexcept = default;
    
    AssetHandle(std::shared_ptr<T> asset) noexcept : asset_(std::move(asset)) {}
    
    /// Get raw pointer to the asset
    [[nodiscard]] T* get() const noexcept { return asset_.get(); }
    
    /// Get raw pointer to the asset (operator)
    [[nodiscard]] T* operator->() const noexcept { return asset_.get(); }
    
    /// Dereference the asset
    [[nodiscard]] T& operator*() const noexcept { return *asset_; }
    
    /// Check if the handle is valid
    [[nodiscard]] bool is_valid() const noexcept { return asset_ != nullptr; }
    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }
    
    /// Get the asset ID
    [[nodiscard]] AssetId id() const noexcept {
        return asset_ ? asset_->id() : AssetId{};
    }
    
    /// Get the reference count
    [[nodiscard]] long use_count() const noexcept { return asset_.use_count(); }
    
    /// Reset the handle
    void reset() noexcept { asset_.reset(); }

private:
    std::shared_ptr<T> asset_;
};

} // namespace avernal
