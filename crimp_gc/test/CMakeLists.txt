file(GLOB_RECURSE TEST_CRIMPGC_SOURCES "*.c" "*/*.c")

# Create test executable
add_executable(test_crimpgc 
    ${TEST_CRIMPGC_SOURCES}
)

# Link against crimpgc library
target_link_libraries(test_crimpgc PRIVATE
    crimpgc
)

# Add custom target to build and run tests
add_custom_target(run_tests
    COMMAND test_crimpgc
    DEPENDS test_crimpgc
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Running tests..."
)

# Python test runner target
add_custom_target(run_tests_python
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py
    DEPENDS test_crimpgc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running tests via Python test runner"
)

#set_target_properties(test_crimpgc PROPERTIES
    #UNITY_BUILD ON
    #UNITY_BUILD_BATCH_SIZE 0  # 0 means unlimited (all in one file)
    # OUTPUT_NAME "test_crimpgc"
#)