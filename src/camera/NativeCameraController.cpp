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



bool gEnabled=false;


bool gInitialCaptured=false;



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
        return true;



    if(
        !installCameraHook()
    )
    {

        LOGE(
            "Camera hook failed"
        );


        return false;

    }



    gCameraState.enabled.store(
        true
    );



    gEnabled=true;


    gInitialCaptured=false;



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



    removeCameraHook();



    gCameraState.enabled.store(
        false
    );


    gCameraState.captured.store(
        false
    );



    mLastCameraComponent.store(
        nullptr
    );



    gInitialCaptured=false;


    gEnabled=false;



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
        gCameraState.captured.load();

}






void NativeCameraController::update()
noexcept
{


    if(!gEnabled)
        return;



}






void NativeCameraController::onCameraTransform(
    void* cameraComponent
)
noexcept
{


    if(
        !gEnabled ||
        cameraComponent==nullptr
    )
    {
        return;
    }



    mLastCameraComponent.store(
        cameraComponent
    );



    auto& state =
        getCameraState();





    if(!gInitialCaptured)
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
                true
            );


            gInitialCaptured=true;


        }


    }




    /*
        TEST FREECAM OFFSET

        X 0
        Y naik 2 blok
        Z mundur 4 blok
    */


    Vec3 offset{};


    offset.x=0.0f;

    offset.y=2.0f;

    offset.z=-4.0f;



    if(
        CameraController::
        instance()
        .applyOffset(
            cameraComponent,
            offset
        )
    )
    {


        LOGI(
            "Virtual camera applied"
        );


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



    state.velocity.x=x;

    state.velocity.y=y;

    state.velocity.z=z;



}






void NativeCameraController::requestRecapture()
noexcept
{

    gInitialCaptured=false;

}





}
