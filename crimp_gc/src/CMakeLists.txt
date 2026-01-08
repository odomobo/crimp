file(GLOB_RECURSE CRIMP_GC_SOURCES "*.c")

# Find pthreads
# TODO: need to handle windows separately...
find_package(Threads REQUIRED)

# Create shared library
add_library(crimp_gc SHARED
    ${CRIMP_GC_SOURCES}
)

# Include directories
target_include_directories(crimp_gc PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Define CRIMP_GC_BUILD when building the library
target_compile_definitions(crimp_gc PRIVATE CRIMP_GC_BUILD)

# Link pthreads
target_link_libraries(crimp_gc PUBLIC
    Threads::Threads
)

set_target_properties(crimp_gc PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 0  # 0 means unlimited (all in one file)
    # OUTPUT_NAME "crimp_gc"
)