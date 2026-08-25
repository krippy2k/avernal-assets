#include <avernal/assets/asset_id.hpp>
#include <cstring>

namespace avernal {

// Simple FNV-1a hash for string to ID conversion
static constexpr std::uint64_t fnv1a_hash(std::string_view str) noexcept {
    std::uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= static_cast<std::uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

AssetId::AssetId(std::string_view path) noexcept
    : id_(fnv1a_hash(path)) {
}

} // namespace avernal
