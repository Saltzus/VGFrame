-- Libraries ------

add_requires("miniaudio")
add_requires("glad")
add_requires("glm")
add_requires("glfw")
add_requires("tinygltf 2.9.7")
add_requires("volk")
add_requires("freetype")
add_requires("nlohmann_json")
add_requires("tracy")
add_requires("box3d")

add_requires("slang v2026.16", {system = false})   
add_requires("imgui 1.92.9", {configs = {glfw = true, opengl3 = true, vulkan = true, volk = true}})

-- ----------------

add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_cxxflags("-frtti", {tools = {"gcc", "clang"}})

-- Options --------

option("examples")
    set_default(false)
    set_description("Build examples")

-- Includes -------

includes("VGF")

if has_config("examples") then
    includes("Examples/*/xmake.lua")
end