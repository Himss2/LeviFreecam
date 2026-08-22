cmake_minimum_required(VERSION 3.22)


project(
    levi_freecam
    LANGUAGES CXX
)





set(
    CMAKE_CXX_STANDARD
    20
)


set(
    CMAKE_CXX_STANDARD_REQUIRED
    ON
)


set(
    CMAKE_CXX_EXTENSIONS
    OFF
)







if(NOT ANDROID)

    message(
        FATAL_ERROR

        "LeviFreecam is an Android native mod. Configure CMake with the Android NDK toolchain."

    )

endif()







include(FetchContent)







FetchContent_Declare(

    preloader_android

    GIT_REPOSITORY
    https://github.com/LiteLDev/preloader-android.git

    GIT_TAG
    main

    GIT_SHALLOW
    TRUE

)






FetchContent_MakeAvailable(
    preloader_android
)









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









add_library(

    levi_freecam

    SHARED





    #
    # Main
    #

    src/FreecamMod.cpp






    #
    # Core
    #

    src/core/FreecamController.cpp

    src/core/PlayerFreezeController.cpp






    #
    # Camera System
    #

    src/camera/CameraController.cpp

    src/camera/NativeCameraController.cpp

    src/camera/CameraState.cpp

    src/camera/CameraHook.cpp

    src/camera/CameraEntityTracker.cpp

    src/camera/CameraSystemHook.cpp






    #
    # Memory Resolver
    #

    src/memory/CameraResolver.cpp


    #
    # Legacy
    #

    src/game/GameModeController.cpp






    #
    # Network / Player Hooks
    #

    src/hooks/PacketHook.cpp

    src/hooks/PlayerHook.cpp






    #
    # UI
    #

    src/ui/ModMenu.cpp


)









set_target_properties(

    levi_freecam

    PROPERTIES

    OUTPUT_NAME

    "levi_freecam"

)









target_include_directories(

    levi_freecam

    PRIVATE

    "${CMAKE_CURRENT_LIST_DIR}/src"

)









target_link_libraries(

    levi_freecam

    PRIVATE

    preloader

)









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









find_library(

    LEVI_FREECAM_DL_LIB

    dl

)






if(LEVI_FREECAM_DL_LIB)


    target_link_libraries(

        levi_freecam

        PRIVATE

        "${LEVI_FREECAM_DL_LIB}"

    )


endif()









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









if(

    CMAKE_BUILD_TYPE STREQUAL "Release"

)


    target_compile_options(

        levi_freecam

        PRIVATE


        -Oz


    )


endif()









target_link_options(

    levi_freecam

    PRIVATE



    -Wl,--gc-sections



    -Wl,-z,max-page-size=16384


)
