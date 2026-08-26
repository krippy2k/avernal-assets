#include <avernal/assets/asset_metadata.hpp>
#include <avernal/core/assert.hpp>
#include <filesystem>

namespace avernal {

bool AssetMetadata::is_outdated() const {
    namespace fs = std::filesystem;
    
    if (path.empty()) {
        return false;
    }
    
    std::error_code ec;
    if (!fs::exists(path, ec)) {
        return true; // File doesn't exist
    }
    
    auto write_time = fs::last_write_time(path, ec);
    if (ec) {
        return true; // Can't determine modification time
    }
    
    // Convert file_time to system_clock time
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        write_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    
    return sctp > last_modified;
}

const char* to_string(AssetType type) noexcept {
    switch (type) {
        case AssetType::unknown:  return "unknown";
        case AssetType::texture:  return "texture";
        case AssetType::mesh:     return "mesh";
        case AssetType::material: return "material";
        case AssetType::shader:   return "shader";
        case AssetType::audio:    return "audio";
        case AssetType::scene:    return "scene";
        case AssetType::prefab:   return "prefab";
    }
    return "unknown";
}

AssetType asset_type_from_extension(std::string_view extension) noexcept {
    if (extension.empty()) {
        return AssetType::unknown;
    }
    
    // Ensure extension starts with '.'
    if (extension[0] != '.') {
        return AssetType::unknown;
    }
    
    // Texture formats
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
        extension == ".bmp" || extension == ".tga" || extension == ".dds") {
        return AssetType::texture;
    }
    
    // Mesh formats
    if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" ||
        extension == ".glb" || extension == ".mesh" || extension == ".avmesh") {
        return AssetType::mesh;
    }
    
    // Material formats
    if (extension == ".mat" || extension == ".material") {
        return AssetType::material;
    }
    
    // Shader formats
    if (extension == ".hlsl" || extension == ".glsl" || extension == ".spv" ||
        extension == ".shader") {
        return AssetType::shader;
    }
    
    // Audio formats
    if (extension == ".wav" || extension == ".mp3" || extension == ".ogg" ||
        extension == ".flac") {
        return AssetType::audio;
    }
    
    // Scene formats
    if (extension == ".scene") {
        return AssetType::scene;
    }
    
    // Prefab formats
    if (extension == ".prefab") {
        return AssetType::prefab;
    }
    
    return AssetType::unknown;
}

} // namespace avernal
