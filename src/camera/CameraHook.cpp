#include "CameraHook.hpp"

#include "NativeCameraController.hpp"

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


static bool installed = false;



bool installCameraHook()
{


    if(installed)
        return true;



    /*
        TEMP IMPLEMENTATION

        menunggu address final:

        CameraInstructionSystemUtil::_tick


        nanti:

        InlineHook(
            address,
            hkCameraTick,
            &originalCameraTick
        )

    */


    installed = true;


    LOGI(
    "Camera hook installed placeholder"
    );


    return true;

}




void removeCameraHook()
{

    if(!installed)
        return;



    /*
        restore hook
    */


    installed = false;


}



}
