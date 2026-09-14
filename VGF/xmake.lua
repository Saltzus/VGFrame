target("VGF")
    set_kind("static")
    add_files("src/**/*.cpp")
    add_headerfiles("src/**/*.h")
    add_includedirs("src", {public = true})
    set_group("VGFrame")
    add_defines("VK_NO_PROTOTYPES", "IMGUI_IMPL_VULKAN_USE_VOLK")
    add_packages
    (
        "miniaudio",
        "glad",
        "glm",
        "glfw",
        "joltphysics",
        "imgui",
        "tinygltf",
        "volk",
        "vulkan-headers",
        "vulkan-validationlayers",
        "ozz-animation",
        "freetype",
        "nlohmann_json",
        "tracy",
        {public = true}
    )