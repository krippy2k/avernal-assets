# avernal-assets

Asset management system for the Avernal game engine.

## Overview

`avernal-assets` provides a comprehensive asset management system with features including:

- **AssetId**: Unique identifiers for assets (path-based or numeric)
- **AssetHandle**: Reference-counted smart pointers for automatic memory management
- **AssetManager**: Central hub for loading, caching, and unloading assets
- **AssetRegistry**: Metadata storage and asset discovery
- **AssetLoader**: Extensible interface for loading different asset types
- **Metadata**: Asset information, dependencies, and file tracking
- **Dependency Management**: Automatic dependency resolution and loading

## Features

### Type-Safe Asset Handles

```cpp
#include <avernal/assets/assets.hpp>

AssetManager manager;

// Load a texture
auto texture = manager.load<TextureAsset>("textures/wall.png");
if (texture) {
    // Use the texture
    auto* tex = texture.get();
}

// Handle automatically unloads when going out of scope
```

### Asset Registry and Discovery

```cpp
AssetRegistry& registry = manager.registry();

// Scan a directory for assets
registry.scan_directory("assets/", true);

// Query assets by type
auto textures = registry.get_assets_by_type(AssetType::texture);

// Get asset metadata
auto metadata = registry.get_metadata(AssetId{"textures/wall.png"});
```

### Custom Asset Loaders

```cpp
class MeshAssetLoader : public IAssetLoader {
public:
    AssetType type() const noexcept override {
        return AssetType::mesh;
    }
    
    std::vector<std::string> extensions() const override {
        return {".obj", ".fbx", ".gltf"};
    }
    
    std::shared_ptr<Asset> load(
        AssetId id,
        std::string_view path,
        const AssetMetadata& metadata
    ) override {
        // Load mesh from file
        auto mesh = std::make_shared<MeshAsset>();
        // ... load data ...
        return mesh;
    }
};

// Register the loader
manager.register_loader(std::make_unique<MeshAssetLoader>());
```

### Dependency Management

```cpp
// Assets can depend on other assets
AssetMetadata material_metadata;
material_metadata.dependencies.push_back(texture_id);
material_metadata.dependencies.push_back(shader_id);

// Dependencies are tracked in the registry
registry.register_asset(material_metadata);
```

### Hot Reloading

```cpp
// Check if asset file has changed
if (metadata.is_outdated()) {
    // Reload the asset
    manager.reload(asset_id);
}
```

### Memory Management

```cpp
// Preload assets for faster access
manager.preload("textures/wall.png");

// Check what's loaded
size_t count = manager.loaded_count();

// Garbage collect unused assets
manager.garbage_collect();

// Clear all cached assets
manager.clear_cache();
```

## Asset Types

Supported asset types:
- `texture` - Images and texture data
- `mesh` - 3D geometry
- `material` - Material definitions
- `shader` - Shader programs
- `audio` - Sound effects and music
- `scene` - Scene definitions
- `prefab` - Reusable game objects

## Thread Safety

`AssetManager` is thread-safe and can be accessed from multiple threads simultaneously. All public methods use internal locking to ensure data consistency.

## Integration Example

```cpp
#include <avernal/assets/assets.hpp>

int main() {
    AssetManager asset_manager;
    
    // Register loaders for different asset types
    asset_manager.register_loader(std::make_unique<TextureAssetLoader>());
    asset_manager.register_loader(std::make_unique<MeshAssetLoader>());
    
    // Scan for assets
    asset_manager.registry().scan_directory("assets/");
    
    // Load and use assets
    auto texture = asset_manager.load<TextureAsset>("assets/textures/wall.png");
    auto mesh = asset_manager.load<MeshAsset>("assets/models/cube.obj");
    
    // Assets are automatically unloaded when handles go out of scope
    
    return 0;
}
```

## Build

This library requires:
- C++23 compiler
- `avernal-core` (sibling directory)

Build with CMake:
```bash
cmake -B build
cmake --build build
```
