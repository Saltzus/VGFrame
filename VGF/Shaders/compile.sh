#!/bin/bash
set -e

for F in *.vert *.frag *.comp *.geom *.tesc *.tese; do
    echo "Compiling $F..."
    "../../VGFrame/Libraries/vulkan/Bin/glslc" "$F" -o "$F.spv"
done

echo "Done!"
exit 0
