#include "camera/CameraEntityTracker.hpp"


#include <android/log.h>


namespace levifreecam::camera {



namespace {


constexpr char kLogTag[] =
    "Levi Freecam Tracker";


}



CameraEntityTracker&
CameraEntityTracker::instance()
noexcept
{

    static CameraEntityTracker tracker;

    return tracker;

}





void CameraEntityTracker::track(
    void* cameraComponent
)
noexcept
{

    if(cameraComponent == nullptr)
    {
        return;
    }



    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "CameraComponent = %p",

        cameraComponent

    );


}



}
