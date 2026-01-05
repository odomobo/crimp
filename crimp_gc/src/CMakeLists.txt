# Find pthreads
find_package(Threads REQUIRED)

# Create static library
add_library(crimp_gc STATIC
    crimp_gc.c
)

# Include directories
target_include_directories(crimp_gc PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link pthreads
target_link_libraries(crimp_gc PUBLIC
    Threads::Threads
)
