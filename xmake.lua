add_requires("miniaudio")
add_requires("glad")
add_requires("glm")
add_requires("glfw")
add_requires("tinygltf 2.9.7")
add_requires("volk")
add_requires("vulkan-validationlayers")
add_requires("freetype")
add_requires("nlohmann_json")
add_requires("tracy")
add_requires("box3d")

package("slang")
    set_kind("binary")
    set_homepage("https://shader-slang.com/")
    set_description("Slang shader compiler (official prebuilt release)")

    if is_host("windows") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-windows-x86_64.zip")
        add_versions("2026.19", "fc922f214b5d115f663932e59e80e0f6b6cc6fae140979cf307da6459b6f8f33")
    elseif is_host("linux") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-linux-x86_64.tar.gz")
        add_versions("2026.19", "bd6cfc47b7353b2cffa36a866b1584749f76f4b2dbebf6dda26bc3fb40dc3c0e")
    end

    on_install("windows|x64", "linux|x86_64", function (package)
        os.cp("*", package:installdir())
    end)

    on_test(function (package)
        os.vrun("slangc -v")
    end)
package_end()

add_requires("slang")

add_requires("imgui 1.92.9+b", {configs = {glfw = true, opengl3 = true, vulkan = true, volk = true}})
add_requires("joltphysics", {configs = {rtti = true, debug_renderer = true}})

add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_cxxflags("-frtti", {tools = {"gcc", "clang"}})

rule("slang")
    set_extensions(".slang")
    on_build_file(function (target, sourcefile, opt)
        import("core.project.depend")
        local absSourceFile = path.absolute(sourcefile)
        local outputfile = absSourceFile:gsub("%.slang$", ".spv")

        local function compile()
            cprint("${dim}compiling${clear} ${bright}%s${clear}", sourcefile)
            local envs = target:pkgenvs()
            os.execv("slangc", {absSourceFile, "-target", "spirv", "-o", outputfile}, {envs = target:pkgenvs()})
        end

        if not os.isfile(outputfile) then
            os.rm(target:dependfile(outputfile))
            compile()
            depend.on_changed(function() end, 
            {
                dependfile = target:dependfile(outputfile),
                files = absSourceFile,
            })
        else
            depend.on_changed(compile, 
            {
                dependfile = target:dependfile(outputfile),
                files = absSourceFile,
            })
        end
    end)
    on_clean(function (target, sourcefile)
        local absSourceFile = path.absolute(sourcefile)
        os.rm(absSourceFile:gsub("%.slang$", ".spv"))
    end)

includes("VGF")