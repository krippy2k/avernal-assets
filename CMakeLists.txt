cmake_minimum_required(VERSION 3.28)

project(
    avernal-assets
    VERSION 0.1.0
    DESCRIPTION "Asset management system for the Avernal game engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(FetchContent)

# Find avernal-core
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../avernal-core/CMakeLists.txt")
    set(AVERNAL_CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../avernal-core")
else()
    set(AVERNAL_CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../avernal-core")
endif()

if(NOT EXISTS "${AVERNAL_CORE_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "avernal-core not found at:\n"
        "  ${AVERNAL_CORE_SOURCE_DIR}\n"
        "Place avernal-core as a sibling of this project.")
endif()

FetchContent_Declare(
    avernal_core
    SOURCE_DIR "${AVERNAL_CORE_SOURCE_DIR}"
    EXCLUDE_FROM_ALL
)

set(AVERNAL_CORE_BUILD_DEMO OFF)

FetchContent_MakeAvailable(avernal_core)

add_library(avernal-assets)
add_library(avernal::assets ALIAS avernal-assets)

target_sources(
    avernal-assets
    PRIVATE
        src/asset_id.cpp
        src/asset_handle.cpp
        src/asset_manager.cpp
        src/asset_registry.cpp
        src/asset_loader.cpp
        src/asset_metadata.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/avernal/assets/assets.hpp
            include/avernal/assets/asset_id.hpp
            include/avernal/assets/asset_handle.hpp
            include/avernal/assets/asset_manager.hpp
            include/avernal/assets/asset_registry.hpp
            include/avernal/assets/asset_loader.hpp
            include/avernal/assets/asset_metadata.hpp
)

target_link_libraries(avernal-assets PUBLIC avernal::core)
target_compile_features(avernal-assets PUBLIC cxx_std_23)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(avernal-assets PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(avernal-assets PRIVATE /W4)
endif()
