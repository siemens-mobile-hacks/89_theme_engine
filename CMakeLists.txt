cmake_minimum_required(VERSION 3.28)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(BUILD_TARGET STREQUAL "elf")
    include(../sdk/config.cmake)
    project(sieelf_89_theme_engine C)
    add_subdirectory(elf)
elseif(BUILD_TARGET STREQUAL "patch")
    set(CMAKE_EXPORT_COMPILE_COMMANDS true)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/patch/toolchain.cmake)
    project(patch_89_theme_engine LANGUAGES C ASM)
    add_subdirectory(patch)
endif()
