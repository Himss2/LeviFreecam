cmake_minimum_required(VERSION 3.22)

project(levi_freecam LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT ANDROID)
    message(FATAL_ERROR
        "LeviFreecam hanya dapat dibuild untuk Android.")
endif()

include(FetchContent)

# ============================================================
# Levi / Preloader Android SDK
#
# PENTING:
# Jangan gunakan tag 0.2.2.
#
# Levi Launcher 1.5.x memakai Mod Menu API yang lebih baru.
# ModuleInfo/ButtonInfo berubah setelah 0.2.2 sehingga binary
# yang dibuild dengan header lama dapat mengalami ABI mismatch
# ketika registerModule() dipanggil.
# ============================================================

FetchContent_Declare(
    preloader_android

    GIT_REPOSITORY
        https://github.com/LiteLDev/preloader-android.git

    GIT_TAG
        main

    GIT_SHALLOW
        TRUE
)

FetchContent_MakeAvailable(preloader_android)

# ============================================================
# Output
# ============================================================

if(ANDROID_ABI)
    set(
        LEVI_FREECAM_OUTPUT_ABI
        "${ANDROID_ABI}"
    )
else()
    set(
        LEVI_FREECAM_OUTPUT_ABI
        "android"
    )
endif()

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/out/${LEVI_FREECAM_OUTPUT_ABI}"
)

# ============================================================
# Levi Freecam
# ============================================================

add_library(
    levi_freecam
    SHARED

    src/FreecamMod.cpp
    src/core/FreecamController.cpp
    src/ui/ModMenu.cpp
)

set_target_properties(
    levi_freecam

    PROPERTIES

    OUTPUT_NAME
        "levi_freecam"
)

# ============================================================
# Include
# ============================================================

target_include_directories(
    levi_freecam

    PRIVATE

    "${CMAKE_CURRENT_LIST_DIR}/src"
)

# ============================================================
# Levi Preloader
# ============================================================

target_link_libraries(
    levi_freecam

    PRIVATE

    preloader
)

# ============================================================
# Android log
# ============================================================

find_library(
    LEVI_FREECAM_LOG_LIB
    log
)

if(LEVI_FREECAM_LOG_LIB)

    target_link_libraries(
        levi_freecam

        PRIVATE

        "${LEVI_FREECAM_LOG_LIB}"
    )

endif()

# ============================================================
# Compiler
# ============================================================

target_compile_options(
    levi_freecam

    PRIVATE

    -Wall
    -Wextra
    -Wpedantic

    -fvisibility=hidden
    -fvisibility-inlines-hidden

    -ffunction-sections
    -fdata-sections
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")

    target_compile_options(
        levi_freecam

        PRIVATE

        -Oz
    )

endif()

# ============================================================
# Linker
# ============================================================

target_link_options(
    levi_freecam

    PRIVATE

    -Wl,--gc-sections

    # Android 16 / 16 KiB page compatibility
    -Wl,-z,max-page-size=16384
)
