add_requires
(
    "miniaudio", 
    "glad",
    "glm",
    "glfw",
    "tinygltf 2.9.7",
    "volk",
    "vulkan-validationlayers",
    "ozz-animation",
    "freetype",
    "nlohmann_json",
    "tracy",
    "imgui 1.92.9+b", {configs = {glfw = true, opengl3 = true, vulkan = true,
        volk = true,   }}
)   

add_requires("joltphysics", {configs = {rtti = true, debug_renderer = true}})

add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_cxxflags("-frtti", {tools = {"gcc", "clang"}})

includes("VGF")