file(GLOB_RECURSE TEST_CRIMP_GC_SOURCES "*.c")

# Create test executable
add_executable(test_crimp_gc 
    ${TEST_CRIMP_GC_SOURCES}
)

# Link against crimp_gc library
target_link_libraries(test_crimp_gc PRIVATE
    crimp_gc
)

# Add custom target to build and run tests
add_custom_target(run_tests
    COMMAND test_crimp_gc
    DEPENDS test_crimp_gc
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Running tests..."
)

set_target_properties(test_crimp_gc PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 0  # 0 means unlimited (all in one file)
    # OUTPUT_NAME "test_crimp_gc"
)