# Include the auto-generated .cmake file
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/st-project.cmake)

# Create an interface library
add_library(ST_PROJECT_INTERFACE INTERFACE)

# Link the interface library with the target from the auto-generated .cmake file
target_link_libraries(ST_PROJECT_INTERFACE INTERFACE ${TARGET_INCLUDE_DIRS})