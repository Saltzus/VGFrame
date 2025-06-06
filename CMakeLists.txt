cmake_minimum_required(VERSION 3.5)
project(VGFrame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIVGF ON)


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

# Disable GLFW extras before fetching the content
set(BUILD_SHARED_LIBS           OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS            OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS                OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS         OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS             OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS         OFF CACHE BOOL "" FORCE)
set(BUILD_COLLISION_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET_EXAMPLE      OFF CACHE BOOL "" FORCE)      # if present in this tag
set(BUILD_PYBULLET              OFF CACHE BOOL "" FORCE)      # if you don’t need Python bindings
set(INSTALL_EXTRA_LIBS          OFF CACHE BOOL "" FORCE)      # skip installing helper libs

FetchContent_Declare(
  bullet
  GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
  GIT_COMMIT        d1a4256b3a019117f2bb6cb8c63d6367aaf512e2 // commit where cmake updated to 3.5
)

FetchContent_MakeAvailable(bullet)



if(TARGET BulletDynamics)
  foreach(_t IN ITEMS BulletDynamics BulletCollision BulletSoftBody LinearMath)
    target_include_directories(${_t} INTERFACE
      "${bullet_SOURCE_DIR}/src"
      "${bullet_SOURCE_DIR}/src/BulletCollision"
      "${bullet_SOURCE_DIR}/src/BulletDynamics"
      "${bullet_SOURCE_DIR}/src/BulletSoftBody"
      "${bullet_SOURCE_DIR}/src/LinearMath"
    )
  endforeach()
endif()

set_target_properties(glad glfw PROPERTIES FOLDER "3rd-party")

set_target_properties(LinearMath BulletCollision BulletDynamics BulletSoftBody Bullet2FileLoader 
                      Bullet3Collision Bullet3Common Bullet3Dynamics Bullet3Geometry Bullet3OpenCL_clew BulletInverseDynamics PROPERTIES FOLDER "3rd-party/bullet3")

# Subdirectories
add_subdirectory(VGF)

add_subdirectory(Examples/HelloWorld)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HelloWorld)
set_target_properties(HelloWorld PROPERTIES FOLDER "Examples")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PVGFEFINED_TARGETS_FOLDER "CMake")
