cmake_minimum_required(VERSION 3.12)
project(VGFrame)

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

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE Libraries/stb)

set(vulkan Libraries/vulkan)
include_directories(${vulkan}/Include)
link_directories(${vulkan}/Lib)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8  # Or latest stable release
)

FetchContent_MakeAvailable(glfw)

set_target_properties(glad glfw PROPERTIES FOLDER "3rd-party")


# Subdirectories

add_subdirectory(VGF)

add_subdirectory(Examples/HelloWorld)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HelloWorld)
set_target_properties(HelloWorld PROPERTIES FOLDER "Examples")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
