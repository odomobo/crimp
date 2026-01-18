file(GLOB_RECURSE CRIMPGC_SOURCES "*.c")

# Find pthreads
# TODO: need to handle windows separately...
find_package(Threads REQUIRED)

# Create shared library
add_library(crimpgc SHARED
    ${CRIMPGC_SOURCES}
)

# Include directories
target_include_directories(crimpgc PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Define CRIMPGC_BUILD when building the library
target_compile_definitions(crimpgc PRIVATE CRIMPGC_BUILD)

# Link pthreads
target_link_libraries(crimpgc PUBLIC
    Threads::Threads
)

set_target_properties(crimpgc PROPERTIES
    #UNITY_BUILD ON
    #UNITY_BUILD_BATCH_SIZE 0  # 0 means unlimited (all in one file)
    # OUTPUT_NAME "crimpgc"
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)