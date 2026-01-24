file(GLOB_RECURSE TESTAPP_CRIMPGC_SOURCES "*.c")

# Create test executable
add_executable(testapp_crimpgc 
    ${TESTAPP_CRIMPGC_SOURCES}
)

# Link against crimpgc library
target_link_libraries(testapp_crimpgc PRIVATE
    crimpgc
)

# Add custom target to build and run tests
add_custom_target(run_testapp
    COMMAND testapp_crimpgc
    DEPENDS testapp_crimpgc
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Running test app..."
)

set_target_properties(testapp_crimpgc PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 0  # 0 means unlimited (all in one file)
    # OUTPUT_NAME "testapp_crimpgc"
)