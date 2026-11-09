-- Prebuilt Slang compiler release (shader-slang/slang).
--
-- vshaderc links the Slang compiler in-process (drives it via the C++ API), rather
-- than shelling out to slangc or building Slang from source. The Vulkan SDK's bundled
-- Slang (2025.11) crashes emitting tessellation hull shaders to SPIR-V
-- ("unimplemented: BuiltinCast in spirv-emit"); 2026.11 fixes it. This package pins a
-- known-good prebuilt so shader compilation is reproducible.
--
-- This is a HOST/offline tool dependency ONLY. The vshadersystem runtime library never
-- links Slang; only the desktop `vshaderc` compiler does. So:
--   * Desktop hosts (Windows/Linux/macOS) need this when building the compiler.
--   * Android/Web are NOT host platforms here; they only consume precompiled binaries,
--     so they never require this package.
-- Per-host prebuilt binaries from the official release (one URL+sha256 per host triple).
-- Mirrored from VRI's local repo (kept in sync with that pin).
package("slang-prebuilt")
    set_kind("library")
    set_homepage("https://github.com/shader-slang/slang")
    set_description("Slang shading language compiler (prebuilt release).")
    set_license("Apache-2.0")

    if is_plat("windows") and is_arch("x64", "x86_64") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-windows-x86_64.zip")
        add_versions("2026.11", "e070e239290108dd7a83094cf72fb94ae753b2d5331adba76874bf17dd5c8b6a")
    elseif is_plat("linux") and is_arch("x86_64") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-linux-x86_64.zip")
        add_versions("2026.11", "d83ef21fc08d2c035ed15b5ce7541f27de8ec8d1db42b27c72bdc1567e5c4b37")
    elseif is_plat("linux") and is_arch("arm64", "aarch64") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-linux-aarch64.zip")
        add_versions("2026.11", "d59c474d0f63c670b2e4a8134d7a95c12a35ea62cbe74e2d00afa71f8134c7d0")
    elseif is_plat("macosx") and is_arch("arm64", "aarch64") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-macos-aarch64.zip")
        add_versions("2026.11", "595f06cb9c1c306609cd61d9ced20dc75d56c0153d7c1bf1e509834edf133c75")
    elseif is_plat("macosx") and is_arch("x86_64") then
        add_urls("https://github.com/shader-slang/slang/releases/download/v$(version)/slang-$(version)-macos-x86_64.zip")
        add_versions("2026.11", "764ae0e20714930db2e319f7e6339b233d4a7d668dc9bfa2f37772838d6ecaaa")
    end

    on_install(function (package)
        os.cp("include/*.h", package:installdir("include"))
        os.cp("lib/*", package:installdir("lib"))
        -- Ship the whole runtime (DLLs/.so + slangc + the standard module dir) so the
        -- linked slang resolves its dependencies and shader compilation just works.
        os.cp("bin/*", package:installdir("bin"))
        package:add("links", "slang")
        package:addenv("PATH", "bin")
    end)