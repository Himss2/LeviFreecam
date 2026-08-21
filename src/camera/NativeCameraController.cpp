#include "camera/NativeCameraController.hpp"

#include "camera/CameraHook.hpp"
#include "camera/CameraController.hpp"


#include <android/log.h>


#define LOG_TAG "LeviFreecam"



#define LOGI(...) \
    __android_log_print( \
        ANDROID_LOG_INFO, \
        LOG_TAG, \
        __VA_ARGS__ \
    )



#define LOGE(...) \
    __android_log_print( \
        ANDROID_LOG_ERROR, \
        LOG_TAG, \
        __VA_ARGS__ \
    )



namespace levifreecam::camera {



namespace {


CameraState gCameraState;



bool gEnabled =
    false;



bool gInitialCaptured =
    false;



}



CameraState&
getCameraState()
noexcept
{
    return gCameraState;
}




NativeCameraController&
NativeCameraController::instance()
noexcept
{

    static NativeCameraController controller;

    return controller;

}




bool NativeCameraController::enable()
noexcept
{

    LOGI(
        "Native camera enable requested"
    );


    if(gEnabled)
    {
        return true;
    }



    if(
        !installCameraHook()
    )
    {

        LOGE(
            "Camera hook installation failed"
        );


        return false;
    }



    gCameraState.enabled.store(
        true,
        std::memory_order_release
    );



    gEnabled =
        true;



    gInitialCaptured =
        false;



    LOGI(
        "Native camera enabled"
    );



    return true;

}






bool NativeCameraController::disable()
noexcept
{

    LOGI(
        "Native camera disable"
    );



    if(!gEnabled)
    {
        return true;
    }



    removeCameraHook();



    gCameraState.enabled.store(
        false,
        std::memory_order_release
    );



    gCameraState.captureRequested.store(
        false,
        std::memory_order_release
    );



    gCameraState.captured.store(
        false,
        std::memory_order_release
    );



    mLastCameraComponent.store(
        nullptr,
        std::memory_order_release
    );



    gCameraState.position = {};

    gCameraState.velocity = {};



    gInitialCaptured =
        false;



    gEnabled =
        false;



    return true;

}





bool NativeCameraController::isEnabled()
const noexcept
{
    return gEnabled;
}





bool NativeCameraController::cameraCaptured()
const noexcept
{

    return
        gCameraState.captured.load(
            std::memory_order_acquire
        );

}





void NativeCameraController::update()
noexcept
{

    if(!gEnabled)
    {
        return;
    }


    /*
     *
     * Movement masuk di sini.
     *
     * Saat ini hanya update state.
     *
     */



    auto& state =
        getCameraState();



    state.position.x +=
        state.velocity.x;



    state.position.y +=
        state.velocity.y;



    state.position.z +=
        state.velocity.z;



}







void NativeCameraController::onCameraTransform(
    void* cameraComponent
)
noexcept
{


    if(
        !gEnabled ||
        cameraComponent == nullptr
    )
    {
        return;
    }



    /*
     * Simpan camera component asli.
     */
    mLastCameraComponent.store(
        cameraComponent,
        std::memory_order_release
    );



    auto& state =
        getCameraState();





    /*
     * Capture posisi kamera asli.
     *
     * Jangan overwrite transform.
     *
     * Karena CameraComponent masih
     * terhubung dengan LocalPlayer.
     */
    if(
        !gInitialCaptured
    )
    {


        if(
            CameraController::
            instance()
            .readPosition(
                cameraComponent,
                state.position
            )
        )
        {


            LOGI(
                "Camera base %.2f %.2f %.2f",
                state.position.x,
                state.position.y,
                state.position.z
            );



            state.captured.store(
                true,
                std::memory_order_release
            );



            gInitialCaptured =
                true;

        }


    }



}







void NativeCameraController::translate(
    float x,
    float y,
    float z
)
noexcept
{

    auto& state =
        getCameraState();



    state.position.x += x;

    state.position.y += y;

    state.position.z += z;



}







void NativeCameraController::requestRecapture()
noexcept
{

    gCameraState.captureRequested.store(
        true,
        std::memory_order_release
    );



    gInitialCaptured =
        false;

}



}
