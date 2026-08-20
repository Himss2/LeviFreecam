#include "NativeCameraController.hpp"

#include "CameraState.hpp"
#include "CameraHook.hpp"

#include <android/log.h>

#include <cstdint>


#define LOG_TAG "Levi Freecam"


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


namespace levifreecam::camera
{


namespace
{

CameraState gCameraState;


/*
 * ==========================================================
 * Temporary resolved address
 *
 * Nanti diganti hasil RE:
 *
 * libminecraftpe.so
 * CameraInstructionSystem::_tick
 *
 * ==========================================================
 */
constexpr uintptr_t kCameraInstructionTickAddress = 0;


}


/*
 * ==========================================================
 * Camera State
 * ==========================================================
 */

CameraState& getCameraState()
{
    return gCameraState;
}



/*
 * ==========================================================
 * Singleton
 * ==========================================================
 */

NativeCameraController&
NativeCameraController::instance()
{

    static NativeCameraController controller;


    return controller;

}



/*
 * ==========================================================
 * Resolve Camera Function
 * ==========================================================
 */

bool NativeCameraController::resolve()
{

    LOGI(
        "Camera resolve START"
    );


    if(mResolved)
    {

        LOGI(
            "Camera already resolved"
        );


        return true;

    }



    /*
     * Temporary:
     *
     * Address belum dimasukkan.
     *
     * Setelah RE final:
     *
     * mCameraInstructionTick =
     *     base +
     *     offset;
     *
     */


    mCameraInstructionTick =
        kCameraInstructionTickAddress;



    if(mCameraInstructionTick == 0)
    {

        LOGE(
            "CameraInstructionSystem::_tick "
            "NOT RESOLVED"
        );


        return false;

    }



    mResolved = true;



    LOGI(
        "Camera resolved at %p",
        reinterpret_cast<void*>(
            mCameraInstructionTick
        )
    );



    return true;

}



/*
 * ==========================================================
 * Enable Camera
 * ==========================================================
 */

bool NativeCameraController::enable()
{

    LOGI(
        "NativeCameraController ENABLE ENTER"
    );



    if(mEnabled)
    {

        LOGI(
            "Native camera already enabled"
        );


        return true;

    }



    if(!resolve())
    {

        LOGE(
            "Native camera resolve FAILED"
        );


        return false;

    }



    if(!installCameraHook())
    {

        LOGE(
            "Camera hook INSTALL FAILED"
        );


        return false;

    }



    gCameraState.enabled =
        true;



    mEnabled =
        true;



    LOGI(
        "Native camera ENABLED"
    );


    return true;

}



/*
 * ==========================================================
 * Disable Camera
 * ==========================================================
 */

bool NativeCameraController::disable()
{

    LOGI(
        "NativeCameraController DISABLE"
    );



    gCameraState.enabled =
        false;



    mEnabled =
        false;



    return true;

}



/*
 * ==========================================================
 * Status
 * ==========================================================
 */

bool NativeCameraController::isEnabled()
    const
{

    return mEnabled;

}



/*
 * ==========================================================
 * Runtime Update
 * ==========================================================
 */

void NativeCameraController::update()
{

    if(!mEnabled)
    {
        return;
    }



    /*
     * Placeholder.
     *
     * Nanti diganti:
     *
     * camera position write
     * camera rotation write
     *
     */



    /*
     * Debug movement test.
     *
     * Jangan dipakai final.
     */

    gCameraState.y += 0.01f;



}



}
