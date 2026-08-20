#include "NativeCameraController.hpp"

#include "CameraState.hpp"
#include "CameraHook.hpp"

#include <android/log.h>
#include <cstdint>


#define LOG_TAG "LeviFreecam"


#define LOGI(...) \
__android_log_print(
ANDROID_LOG_INFO,
LOG_TAG,
__VA_ARGS__)


#define LOGE(...) \
__android_log_print(
ANDROID_LOG_ERROR,
LOG_TAG,
__VA_ARGS__)



namespace levifreecam::camera
{


namespace
{


CameraState gCameraState;


/*
 * Nanti hasil RE
 */
constexpr uintptr_t kCameraAddress = 0;



bool gResolved = false;

bool gEnabled = false;



}




CameraState& getCameraState() noexcept
{

    return gCameraState;

}




NativeCameraController&
NativeCameraController::instance() noexcept
{

    static NativeCameraController instance;

    return instance;

}





bool NativeCameraController::resolve() noexcept
{

    LOGI(
        "Camera resolve start"
    );


    if(gResolved)
    {
        return true;
    }



    if(kCameraAddress == 0)
    {

        LOGE(
            "Camera address not found"
        );


        return false;

    }



    gResolved = true;



    LOGI(
        "Camera resolved"
    );


    return true;

}





bool NativeCameraController::enable() noexcept
{

    LOGI(
        "Camera enable"
    );



    if(gEnabled)
    {
        return true;
    }



    if(!resolve())
    {

        LOGE(
            "Resolve failed"
        );


        return false;

    }



    if(!installCameraHook())
    {

        LOGE(
            "Hook failed"
        );


        return false;

    }



    gCameraState.enabled = true;


    gEnabled = true;



    LOGI(
        "Camera enabled"
    );



    return true;

}





bool NativeCameraController::disable() noexcept
{

    LOGI(
        "Camera disable"
    );



    gCameraState.enabled = false;


    gEnabled = false;



    return true;

}





bool NativeCameraController::isEnabled() const noexcept
{

    return gEnabled;

}





void NativeCameraController::update() noexcept
{

    if(!gEnabled)
    {
        return;
    }



    /*
       Runtime update nanti
       masuk sini
    */


}




}
