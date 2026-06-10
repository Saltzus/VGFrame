
# Recursively gather all .cpp and .h files in the src directory.
file(GLOB_RECURSE CLIENT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h"
)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/imgui")

set(3rdPartyImGui
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui.h"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_internal.h"
    "${IMGUI_DIR}/imstb_rectpack.h"
    "${IMGUI_DIR}/imstb_textedit.h"
    "${IMGUI_DIR}/imstb_truetype.h"
    
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.h"
    "${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_vulkan.h"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.h"

)

add_library(ImGuiTarget OBJECT ${3rdPartyImGui})
target_link_libraries(ImGuiTarget PRIVATE glfw vulkan glad)
target_include_directories(ImGuiTarget PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries
    ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glad/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)


# Create the executable.
add_library(VGF STATIC ${CLIENT_FILES} $<TARGET_OBJECTS:ImGuiTarget>)
target_compile_definitions(VGF PRIVATE JPH_DEBUG_RENDERER)

target_link_libraries(VGF
    PUBLIC
    glad
    glm
    miniaudio
    nlohmann_json
    stb_image_write
    stb_image
    vulkan
    glfw
    tinygltf
    freetype
    Jolt
)

set_target_properties(VGF PROPERTIES FOLDER "VGFrame")
set_target_properties(VGF PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/Core.h)

target_include_directories(VGF PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(VGF PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/../
  ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries
  ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glad/include
  ${CORE_LIB_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/imgui/backends
)

source_group(VGFrame/3rd-party/imgui FILES ${3rdPartyImGui})

# Function to automatically assign source groups based on folder structure.
function(assign_source_groups)
    foreach(file IN LISTS ARGN)
        # Ensure we have an absolute file path.
        get_filename_component(file_abs "${file}" ABSOLUTE)
        # Get the file path relative to the src directory.
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/" "${file_abs}")
        # Get the directory part (if any) from the relative path.
        get_filename_component(dir_path "${rel_path}" DIRECTORY)
        # Replace forward slashes with backslashes for Visual Studio.
        string(REPLACE "/" "\\" group "${dir_path}")
        # Create the source group for the file.
        source_group("${group}" FILES "${file_abs}")
    endforeach()
endfunction()

# Call the function by expanding the CLIENT_FILES list.
assign_source_groups(${CLIENT_FILES})


