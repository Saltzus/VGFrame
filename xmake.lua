set_project("VGFrame")

add_requires
(
    "miniaudio", 
    "glad",
    "glm",
    "glfw",
    "tinygltf 2.9.7",
    "volk",
    "ozz-animation",
    "freetype",
    "nlohmann-json",
    "tracy",
    "imgui", {configs = {glfw = true, opengl3 = true, vulkan = true}}
)   

add_requires("joltphysics", {configs = {rtti = true, debug_renderer = true}})

add_rules("mode.debug", "mode.release")
set_languages("c++20")
add_cxxflags("-frtti", {force = true})

includes("VGF")