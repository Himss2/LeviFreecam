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

    auto& state = getCameraState();


    if(state.enabled.load())
    {
        return true;
    }


    state.captureRequested.store(true);

    state.captured.store(false);


    state.enabled.store(true);



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

    auto& state = getCameraState();


    void* camera =
        mLastCameraComponent.load();



    if(camera)
    {

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



    mLastCameraComponent.store(nullptr);



    state.enabled.store(false);

    state.captured.store(false);

    state.captureRequested.store(false);



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
        .store(true);
}





void NativeCameraController::onCameraTransform(
    void* cameraComponent
)
noexcept
{


    if(cameraComponent == nullptr)
    {
        return;
    }



    auto& state =
        getCameraState();



    if(!state.enabled.load())
    {
        return;
    }



    mLastCameraComponent.store(
        cameraComponent
    );




    /*
        Capture awal
    */

    if(state.captureRequested.load())
    {


        Vec3 pos{};

        CameraOrientation rot{};



        bool posOK =
            CameraController::
            instance()
            .readPosition(
                cameraComponent,
                pos
            );



        bool rotOK =
            CameraController::
            instance()
            .readOrientation(
                cameraComponent,
                rot
            );




        if(posOK)
        {

            state.originalPosition =
                pos;


            state.position =
                pos;

        }




        if(rotOK)
        {

            state.originalOrientation =
                rot;


            state.orientation =
                rot;

        }



        state.captureRequested.store(false);

        state.captured.store(true);



        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Camera captured"
        );

    }





    /*
        Override kamera asli
    */

    if(state.captured.load())
    {


        CameraController::
        instance()
        .setTransform(
            cameraComponent,
            state.position,
            state.orientation
        );


    }



}






void NativeCameraController::update()
noexcept
{

    auto& state =
        getCameraState();



    if(!state.enabled.load())
    {
        return;
    }



    if(!state.captured.load())
    {
        return;
    }



    /*
        movement sementara

        nanti diganti:
        touch
        joystick
        keyboard
    */


    state.position.x +=
        state.velocity.x;


    state.position.y +=
        state.velocity.y;


    state.position.z +=
        state.velocity.z;



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



    if(!state.enabled.load())
    {
        return;
    }



    state.velocity.x = x;

    state.velocity.y = y;

    state.velocity.z = z;



}





}
