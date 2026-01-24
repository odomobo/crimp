cmake_minimum_required(VERSION 3.10)
project(crimpgc C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_IWYU "-O0 -g")

mark_as_advanced(
  CMAKE_C_FLAGS_IWYU
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING 
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel IWYU."
        FORCE
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C flags: ${CMAKE_C_FLAGS}")

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(testapp)