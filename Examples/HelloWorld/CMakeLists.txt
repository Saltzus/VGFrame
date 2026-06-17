
# Recursively gather all .cpp and .h files in the src directory.
file(GLOB_RECURSE CLIENT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.h"
)

# Create the executable.
add_executable(HelloWorld ${CLIENT_FILES})

target_link_libraries(HelloWorld PRIVATE VGFrame)

target_include_directories(HelloWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

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


