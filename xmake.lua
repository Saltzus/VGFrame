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

add_requires("slang")

add_requires("imgui 1.92.9", {configs = {glfw = true, opengl3 = true, vulkan = true, volk = true}})

-- ----------------

add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_cxxflags("-frtti", {tools = {"gcc", "clang"}})

-- Options --------

option("examples")
    set_default(false)
    set_description("Build examples")

-- Rules ----------

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

-- Includes -------

includes("VGF")

if has_config("examples") then
    includes("Examples/*/xmake.lua")
end