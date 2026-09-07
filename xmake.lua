set_project("VGFrame")

add_requires
(
    "miniaudio", 
    "glad",
    "glm",
    "glfw",
    "joltphysics",
    "imgui",
    "tinygltf 2.9.7",
    "volk",
    "ozz-animation",
    "freetype"
)

add_rules("mode.debug", "mode.release")
set_languages("c++20")

includes("VGF")