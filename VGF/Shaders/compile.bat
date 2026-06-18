@echo off
setlocal enabledelayedexpansion

for %%F in (*.vert *.frag *.comp *.geom *.tesc *.tese) do (
    echo Compiling %%F...
    "..\..\Libraries\vulkan\Bin\glslc.exe" "%%F" -o "%%F.spv"
)

for %%F in (*_vert.slang *_frag.slang) do (
    echo Compiling %%F...
    "..\..\Libraries\vulkan\Bin\slangc.exe" "%%F" -profile glsl_450 -target spirv -o "%%~nF.spv"
    "..\..\Libraries\vulkan\Bin\slangc.exe" "%%F" -entry main  -profile glsl_450 -target glsl -o "%%~nF.glsl"
)

echo Done!
pause
