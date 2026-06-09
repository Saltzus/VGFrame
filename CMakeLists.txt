cmake_minimum_required(VERSION 3.5)
project(VGFrame)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Distribution")


cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Libraries

add_library(glad STATIC Libraries/glad/src/glad.c)
target_include_directories(glad PUBLIC Libraries/glad/include)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE Libraries/glm)

add_library(miniaudio INTERFACE)
target_include_directories(miniaudio INTERFACE Libraries/miniaudio)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE Libraries/nlohmann_json)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE Libraries/stb)

add_library(stb_image_write INTERFACE)
target_include_directories(stb_image_write INTERFACE Libraries/stb)

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE Libraries/tinygltf)

add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/vulkan/Include
)
target_link_directories(vulkan INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/vulkan/Lib
)
target_link_libraries(vulkan INTERFACE vulkan-1)

include(FetchContent)

# Disable GLFW extras before fetching the content
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)  # Optional: don't generate install targets

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8  # Use the version you want
)

FetchContent_MakeAvailable(glfw)


# When turning this option on, the library will be compiled using doubles for positions. This allows for much bigger worlds.
set(DOUBLE_PRECISION OFF)
# When turning this option on, the library will be compiled with debug symbols
set(GENERATE_DEBUG_SYMBOLS ON)
# When turning this option on, the library will override the default CMAKE_CXX_FLAGS_DEBUG/RELEASE values, otherwise they will use the platform defaults
set(OVERRIDE_CXX_FLAGS OFF)
# When turning this option on, the library will be compiled in such a way to attempt to keep the simulation deterministic across platforms
set(CROSS_PLATFORM_DETERMINISTIC OFF)
# When turning this option on, the library will be compiled with interprocedural optimizations enabled, also known as link-time optimizations or link-time code generation.
# Note that if you turn this on you need to use SET_INTERPROCEDURAL_OPTIMIZATION() or set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON) to enable LTO specifically for your own project as well.
# If you don't do this you may get an error: /usr/bin/ld: libJolt.a: error adding symbols: file format not recognized
set(INTERPROCEDURAL_OPTIMIZATION OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF) 
# When turning this on, in Debug and Release mode, the library will emit extra code to ensure that the 4th component of a 3-vector is kept the same as the 3rd component 
# and will enable floating point exceptions during simulation to detect divisions by zero. 
# Note that this currently only works using MSVC. Clang turns Float2 into a SIMD vector sometimes causing floating point exceptions (the option is ignored).
set(FLOATING_POINT_EXCEPTIONS_ENABLED OFF)
# When turning this on, the library will be compiled with C++ exceptions enabled.
# This adds some overhead and Jolt doesn't use exceptions so by default it is off.
set(CPP_EXCEPTIONS_ENABLED OFF)
# When turning this on, the library will be compiled with C++ RTTI enabled.
# This adds some overhead and Jolt doesn't use RTTI so by default it is off.
set(CPP_RTTI_ENABLED OFF)
# Number of bits to use in ObjectLayer. Can be 16 or 32.
set(OBJECT_LAYER_BITS 16)
# Select X86 processor features to use, by default the library compiles with AVX2, if everything is off it will be SSE2 compatible.
set(USE_SSE4_1 ON)
set(USE_SSE4_2 ON)
set(USE_AVX ON)
set(USE_AVX2 ON)
set(USE_AVX512 OFF)
set(USE_LZCNT ON)
set(USE_TZCNT ON)
set(USE_F16C ON)
set(USE_FMADD ON)

# Include Jolt
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.5.0"
		SOURCE_SUBDIR "Build"
)

set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(JoltPhysics)

# FreeType
set(FT_DISABLE_ZLIB        ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2       ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG         ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ    ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI      ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL       ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG        VER-2-13-3
)

FetchContent_MakeAvailable(freetype)

set_target_properties(glad glfw freetype update_mappings Jolt PROPERTIES FOLDER "VGFrame/3rd-party")

# Subdirectories
add_subdirectory(VGF)

add_library(VGFrame INTERFACE)
add_dependencies(VGFrame VGF)
target_link_libraries(VGFrame
    INTERFACE
    VGF
)


add_subdirectory(Examples/HelloWorld)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HelloWorld)
set_target_properties(HelloWorld PROPERTIES FOLDER "VGFrame/Examples")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
