# Example: Using the Asset System

```cpp
#include <avernal/assets/assets.hpp>
#include <iostream>

// Define a custom asset type
class TextureAsset : public avernal::Asset {
public:
    int width{}, height{};
    // ... texture data ...
};

// Create a custom loader
class MyTextureLoader : public avernal::IAssetLoader {
public:
    avernal::AssetType type() const noexcept override {
        return avernal::AssetType::texture;
    }
    
    std::vector<std::string> extensions() const override {
        return {".png", ".jpg"};
    }
    
    std::shared_ptr<avernal::Asset> load(
        avernal::AssetId id,
        std::string_view path,
        const avernal::AssetMetadata& metadata
    ) override {
        auto texture = std::make_shared<TextureAsset>();
        // Load texture from file...
        std::cout << "Loading texture: " << path << "\n";
        return texture;
    }
};

int main() {
    avernal::AssetManager manager;
    
    // Register texture loader
    manager.register_loader(std::make_unique<MyTextureLoader>());
    
    // Scan assets directory
    manager.registry().scan_directory("assets/textures");
    
    // Load a texture
    auto texture = manager.load<TextureAsset>("assets/textures/wall.png");
    
    if (texture) {
        std::cout << "Texture loaded successfully!\n";
        std::cout << "Reference count: " << texture.use_count() << "\n";
    }
    
    // Query assets
    auto all_textures = manager.registry().get_assets_by_type(avernal::AssetType::texture);
    std::cout << "Found " << all_textures.size() << " textures\n";
    
    // Texture automatically unloaded when handle goes out of scope
    return 0;
}
```

## Building the Example

```cmake
cmake_minimum_required(VERSION 3.28)
project(asset-example)

set(CMAKE_CXX_STANDARD 23)

# Add avernal-assets as a dependency
add_subdirectory(path/to/avernal-assets)

add_executable(asset-example main.cpp)
target_link_libraries(asset-example PRIVATE avernal::assets)
```
