#include "camera/NativeCameraController.hpp"


#include "camera/CameraController.hpp"
#include "memory/CameraResolver.hpp"


#include <android/log.h>


namespace levifreecam::camera {


namespace {


constexpr char kLogTag[] =
    "Levi Freecam";


}


NativeCameraController&
NativeCameraController::instance() noexcept
{

    static NativeCameraController controller;

    return controller;

}





bool NativeCameraController::enable() noexcept
{

    auto& state =
        getCameraState();


    state.enabled.store(
        true
    );


    state.captureRequested.store(
        true
    );


    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Native camera enabled"
    );


    return true;

}





bool NativeCameraController::disable() noexcept
{

    auto& state =
        getCameraState();


    void* camera =
        mLastCameraComponent.load();


    if(
        camera != nullptr
    )
    {

        CameraController::
        instance()
        .writePosition(
            camera,
            state.position
        );


        CameraController::
        instance()
        .writeOrientation(
            camera,
            state.orientation
        );

    }



    mLastCameraComponent.store(
        nullptr
    );


    state.enabled.store(
        false
    );


    state.captured.store(
        false
    );


    state.captureRequested.store(
        false
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Native camera disabled"
    );


    return true;

}





bool NativeCameraController::isEnabled()
const noexcept
{

    return getCameraState()
        .enabled
        .load();

}





bool NativeCameraController::cameraCaptured()
const noexcept
{

    return getCameraState()
        .captured
        .load();

}





void NativeCameraController::requestRecapture()
noexcept
{

    getCameraState()
        .captureRequested
        .store(
            true
        );

}





void NativeCameraController::onCameraTransform(
    void* cameraComponent
) noexcept
{


    if(
        cameraComponent == nullptr
    )
    {
        return;
    }



    auto& state =
        getCameraState();



    if(
        !state.enabled.load()
    )
    {
        return;
    }




    mLastCameraComponent.store(
        cameraComponent
    );




    if(
        state.captureRequested.load()
    )
    {


        Vec3 currentPosition{};


        CameraOrientation currentOrientation{};



        if(
            CameraController::
            instance()
            .readPosition(
                cameraComponent,
                currentPosition
            )
        )
        {

            state.position =
                currentPosition;

        }



        if(
            CameraController::
            instance()
            .readOrientation(
                cameraComponent,
                currentOrientation
            )
        )
        {

            state.orientation =
                currentOrientation;

        }



        state.captureRequested.store(
            false
        );


        state.captured.store(
            true
        );



        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Camera captured"
        );

    }





    /*
     * Apply virtual camera position
     */

    CameraController::
    instance()
    .writePosition(
        cameraComponent,
        state.position
    );



    CameraController::
    instance()
    .writeOrientation(
        cameraComponent,
        state.orientation
    );


}





void NativeCameraController::update()
noexcept
{


    auto& state =
        getCameraState();


    if(
        !state.enabled.load()
    )
    {
        return;
    }



    if(
        !state.captured.load()
    )
    {
        return;
    }



    /*
     * Movement controller
     *
     * Akan masuk disini:
     *
     * joystick
     * keyboard
     * touch drag
     *
     */



}





void NativeCameraController::translate(
    float x,
    float y,
    float z
) noexcept
{


    auto& state =
        getCameraState();



    if(
        !state.enabled.load()
    )
    {
        return;
    }



    state.position.x += x;

    state.position.y += y;

    state.position.z += z;


}



}
