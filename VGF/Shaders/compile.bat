@echo off
setlocal enabledelayedexpansion

for %%F in (*.vert *.frag *.comp *.geom *.tesc *.tese) do (
    echo Compiling %%F...
    "..\..\VGFrame\Libraries\vulkan\Bin\glslc.exe" "%%F" -o "%%F.spv"
)

echo Done!
pause
