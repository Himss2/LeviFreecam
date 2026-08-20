cmake_minimum_required(VERSION 3.22)

project(levi_freecam LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT ANDROID)
    message(FATAL_ERROR "LeviFreecam is an Android native mod. Configure CMake with the Android NDK toolchain.")
endif()

include(FetchContent)

# Keep the SDK pinned so builds stay reproducible.
FetchContent_Declare(
    preloader_android
    GIT_REPOSITORY https://github.com/LiteLDev/preloader-android.git
    GIT_TAG 0.2.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(preloader_android)

if(ANDROID_ABI)
    set(LEVI_FREECAM_OUTPUT_ABI "${ANDROID_ABI}")
else()
    set(LEVI_FREECAM_OUTPUT_ABI "android")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/out/${LEVI_FREECAM_OUTPUT_ABI}")

add_library(levi_freecam SHARED
    src/FreecamMod.cpp
    src/core/FreecamController.cpp
    src/ui/ModMenu.cpp
)

set_target_properties(levi_freecam PROPERTIES
    OUTPUT_NAME "levi_freecam"
)

target_include_directories(levi_freecam PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src"
)

target_link_libraries(levi_freecam PRIVATE preloader)

find_library(LEVI_FREECAM_LOG_LIB log)
if(LEVI_FREECAM_LOG_LIB)
    target_link_libraries(levi_freecam PRIVATE "${LEVI_FREECAM_LOG_LIB}")
endif()

# Keep the first build small and compatible with Android devices using 16 KiB pages.
target_compile_options(levi_freecam PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    -ffunction-sections
    -fdata-sections
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(levi_freecam PRIVATE -Oz)
endif()

target_link_options(levi_freecam PRIVATE
    -Wl,--gc-sections
    -Wl,-z,max-page-size=16384
)
