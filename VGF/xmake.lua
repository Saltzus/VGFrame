target("VGF")
    set_kind("static")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
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
        {public = true}
    )