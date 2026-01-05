# Create test executable
add_executable(test_crimp_gc
    test.c
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
