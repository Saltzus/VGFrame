target("VGF")
    set_kind("static")
    add_files("src/**/*.cpp")
    add_headerfiles("src/**/*.h")
    add_includedirs("src", {public = true})
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
        "ozz-animation",
        "freetype",
        "nlohmann-json",
        "tracy",
        {public = true}
    )