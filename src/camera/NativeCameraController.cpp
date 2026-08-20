#include "NativeCameraController.hpp"

#include "CameraState.hpp"
#include "CameraHook.hpp"

#include <android/log.h>


#define LOG_TAG "LeviFreecam"


#define LOGI(...) \
__android_log_print(
ANDROID_LOG_INFO,
LOG_TAG,
__VA_ARGS__
)


namespace levifreecam::camera
{


static CameraState gCameraState;


CameraState& getCameraState()
{
    return gCameraState;
}



NativeCameraController&
NativeCameraController::instance()
{
    static NativeCameraController controller;

    return controller;
}



bool NativeCameraController::resolve()
{

    if(mResolved)
        return true;


    /*
        TODO:

        Resolve:

        _ZN27CameraInstructionSystemUtil5_tickE


        setelah address ARM64 final ditemukan,
        resolver ini akan mengembalikan address
    */


    mCameraInstructionTick = 0;


    if(mCameraInstructionTick == 0)
    {

        LOGI(
        "CameraInstructionSystem::_tick not resolved"
        );


        return false;
    }



    mResolved = true;


    return true;
}



bool NativeCameraController::enable()
{

    if(mEnabled)
        return true;



    if(!resolve())
    {
        LOGI(
        "Native camera resolve failed"
        );

        return false;
    }



    if(!installCameraHook())
    {
        LOGI(
        "Camera hook failed"
        );

        return false;
    }



    gCameraState.enabled = true;


    mEnabled = true;



    LOGI(
    "Native camera enabled"
    );


    return true;
}




bool NativeCameraController::disable()
{

    gCameraState.enabled = false;


    mEnabled = false;


    LOGI(
    "Native camera disabled"
    );


    return true;
}




bool NativeCameraController::isEnabled() const
{
    return mEnabled;
}




void NativeCameraController::update()
{

    if(!mEnabled)
        return;



    /*
        tahap pertama:

        test camera offset

        nanti diganti:

        CameraInstruction
        position
        rotation

    */


    gCameraState.y += 5.0f;

}



}
