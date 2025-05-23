set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# specify the compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set macOS specific flags
set(CMAKE_C_FLAGS_INIT "-arch arm64 -mmacosx-version-min=11.0")
set(CMAKE_CXX_FLAGS_INIT "-arch arm64 -mmacosx-version-min=11.0")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-framework AppKit")

# Set include directories
include_directories(SYSTEM ${TARGET_PREFIX}/include)

# Set OSX deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for macOS")

# skip framework checks
set(CMAKE_FIND_FRAMEWORK NEVER) 
set(CMAKE_STRIP strip)
