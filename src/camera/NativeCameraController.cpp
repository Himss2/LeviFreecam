#include "camera/NativeCameraController.hpp"


#include "camera/CameraController.hpp"


#include <android/log.h>



namespace levifreecam::camera {



namespace {


constexpr char kLogTag[] =
    "Levi Freecam";



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

    auto& state =
        getCameraState();



    if(
        state.enabled.load()
    )
    {
        return true;
    }



    state.captureRequested.store(
        true
    );



    state.enabled.store(
        true
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Native camera enabled"
    );



    return true;

}






bool NativeCameraController::disable()
noexcept
{

    auto& state =
        getCameraState();



    void* camera =
        mLastCameraComponent.load();



    if(
        camera != nullptr
    )
    {


        /*
         * Restore kamera asli Minecraft
         */

        CameraController::
        instance()
        .setTransform(

            camera,

            state.originalPosition,

            state.originalOrientation

        );


        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Camera restored"
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
)
noexcept
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







    /*
     * Capture pertama kali
     *
     * Simpan transform kamera asli
     */

    if(
        state.captureRequested.load()
    )
    {


        Vec3 position{};

        CameraOrientation orientation{};




        bool positionOK =
            CameraController::
            instance()
            .readPosition(

                cameraComponent,

                position

            );



        bool rotationOK =
            CameraController::
            instance()
            .readOrientation(

                cameraComponent,

                orientation

            );




        if(
            positionOK
        )
        {

            state.originalPosition =
                position;


            state.position =
                position;

        }





        if(
            rotationOK
        )
        {

            state.originalOrientation =
                orientation;


            state.orientation =
                orientation;

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
     * Jika freecam aktif
     *
     * Override kamera Minecraft
     */

    CameraController::
    instance()
    .setTransform(

        cameraComponent,

        state.position,

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
     *
     * Movement integration point
     *
     * Nanti masuk:
     *
     * joystick
     * touch drag
     * keyboard
     *
     */



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
