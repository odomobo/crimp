# crimp_gc

A garbage collector library written in C17.

## Project Structure

- `include/` - Public API headers
- `src/` - Library implementation (builds to `libcrimp_gc.a`)
- `test/` - Test executable that links against the library
- `build/` - CMake build directory (out-of-tree builds)
  - `build/lib/` - Static library output
  - `build/bin/` - Test executable output

## Build System

Uses CMake with:
- C17 standard
- Unity builds enabled (all source files compiled together)
- pthreads dependency
- Compiler flags: `-Wall -Wextra -g`

### Build Commands
```bash
cmake -B build
cmake --build build
make run_tests  # Build and run tests
```

## API Overview

The garbage collector provides:

- **Handles** - Reference-counted handles for read-only access to GC-managed objects
- **Builders** - Mutable handles for constructing objects
- **Frames** - Stack frame management for GC roots
- **Slots** - GC-managed memory slots with write barriers
- **Types** - Type descriptors with size information
- **Collection** - Manual GC triggering and allocation functions

### Key Types

- `crimp_gc_handle` - Read-only handle with refcount
- `crimp_gc_builder` - Mutable builder handle
- `crimp_gc_slot` - Opaque pointer to GC-managed slot
- `crimp_type` - Type descriptor (contains size, TODO: mark function)

## Implementation Status

Early development. API defined in `include/crimp_gc.h` but implementation is minimal.

## Notes

- Nothing is finalized yet
- Windows threading handling needs consideration (see TODO in src/CMakeLists.txt)
