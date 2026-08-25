#include <avernal/assets/asset_registry.hpp>
#include <avernal/core/assert.hpp>
#include <filesystem>
#include <fstream>

namespace avernal {

void AssetRegistry::register_asset(const AssetMetadata& metadata) {
    AV_ASSERT(metadata.id.is_valid());
    assets_[metadata.id] = metadata;
    if (!metadata.path.empty()) {
        path_to_id_[metadata.path] = metadata.id;
    }
}

void AssetRegistry::unregister_asset(AssetId id) {
    auto it = assets_.find(id);
    if (it != assets_.end()) {
        if (!it->second.path.empty()) {
            path_to_id_.erase(it->second.path);
        }
        assets_.erase(it);
    }
}

std::optional<AssetMetadata> AssetRegistry::get_metadata(AssetId id) const {
    auto it = assets_.find(id);
    if (it != assets_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool AssetRegistry::is_registered(AssetId id) const {
    return assets_.find(id) != assets_.end();
}

std::vector<AssetMetadata> AssetRegistry::get_assets_by_type(AssetType type) const {
    std::vector<AssetMetadata> result;
    for (const auto& [id, metadata] : assets_) {
        if (metadata.type == type) {
            result.push_back(metadata);
        }
    }
    return result;
}

std::vector<AssetMetadata> AssetRegistry::get_all_assets() const {
    std::vector<AssetMetadata> result;
    result.reserve(assets_.size());
    for (const auto& [id, metadata] : assets_) {
        result.push_back(metadata);
    }
    return result;
}

void AssetRegistry::scan_directory(const std::string& path, bool recursive) {
    namespace fs = std::filesystem;
    
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec)) {
        return;
    }
    
    auto scan_file = [this](const fs::path& file_path) {
        std::string path_str = file_path.string();
        AssetId id{path_str};
        
        // Skip if already registered
        if (is_registered(id)) {
            return;
        }
        
        AssetMetadata metadata;
        metadata.id = id;
        metadata.path = path_str;
        metadata.type = asset_type_from_extension(file_path.extension().string());
        
        std::error_code ec;
        metadata.file_size = fs::file_size(file_path, ec);
        
        auto write_time = fs::last_write_time(file_path, ec);
        if (!ec) {
            metadata.last_modified = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                write_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
        }
        
        register_asset(metadata);
    };
    
    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(path, ec)) {
            if (entry.is_regular_file(ec)) {
                scan_file(entry.path());
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(path, ec)) {
            if (entry.is_regular_file(ec)) {
                scan_file(entry.path());
            }
        }
    }
}

bool AssetRegistry::save_to_file(const std::string& path) const {
    // TODO: Implement serialization (JSON, binary, etc.)
    // This is a placeholder for future implementation
    return false;
}

bool AssetRegistry::load_from_file(const std::string& path) {
    // TODO: Implement deserialization
    // This is a placeholder for future implementation
    return false;
}

void AssetRegistry::clear() {
    assets_.clear();
    path_to_id_.clear();
}

} // namespace avernal
