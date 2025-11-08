# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project("Remote execution helper"
        VERSION 0.1
        DESCRIPTION "Remote execution helper, accepts remote exec and runs it at host machine, C++20"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prevent macOS-specific flags
set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "" CACHE STRING "" FORCE)
set(CMAKE_OSX_SYSROOT "" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compile options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring for Debug build")
    add_compile_options(
        -O0
        -g
        -fconcepts-diagnostics-depth=3
        -fdiagnostics-all-candidates
    )
else()
    message(STATUS "Configuring for Release build")
    add_compile_options(
        -O3
        -fvisibility=hidden
        -fmerge-all-constants
        -fno-ident
    )
endif()

add_compile_options(
    -Wall
    -Werror
    -Wextra
    -Wpedantic
    
    -march=x86-64-v2
    -mtune=generic
)

# Static linking
function(add_static_link_flags target)
    target_link_options(${target} PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endfunction()

add_subdirectory(src)