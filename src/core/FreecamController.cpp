#include "core/FreecamController.hpp"

#include "camera/NativeCameraController.hpp"

#include <android/log.h>


namespace levifreecam {


namespace {

constexpr char kLogTag[] =
    "Levi Freecam";


}



FreecamController&
FreecamController::instance()
{
    static FreecamController controller;

    return controller;
}





void FreecamController::setModuleEnabled(
    bool enabled
)
noexcept
{

    mModuleEnabled.store(
        enabled
    );


    if(!enabled)
    {

        mRequestedActive.store(
            false
        );


        restoreNow();


        camera::
        NativeCameraController::
        instance()
        .disable();



        PlayerFreezeController::
        instance()
        .disable();



        clearSessionState();

    }

}







void FreecamController::setActive(
    bool active
)
noexcept
{


    if(
        active &&
        !moduleEnabled()
    )
    {
        return;
    }



    mRequestedActive.store(
        active
    );



    if(active)
    {

        camera::
        NativeCameraController::
        instance()
        .enable();



        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Native Freecam enabled"
        );

    }
    else
    {

        restoreNow();


        camera::
        NativeCameraController::
        instance()
        .disable();



        PlayerFreezeController::
        instance()
        .disable();



        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Native Freecam disabled"
        );

    }


}







void FreecamController::onLocalPlayerTick(
    void* localPlayer
)
noexcept
{

    if(localPlayer == nullptr)
        return;



    mCurrentPlayer.store(
        localPlayer
    );



    if(!active())
    {
        return;
    }



    /*
     * Freeze player hanya saat freecam aktif
     */
    PlayerFreezeController::
    instance()
    .enable(
        localPlayer
    );



    PlayerFreezeController::
    instance()
    .tick(
        localPlayer
    );



    /*
     * Update virtual camera
     */
    camera::
    NativeCameraController::
    instance()
    .update();


}







bool FreecamController::restoreNow()
noexcept
{

    /*
     * Native camera controller
     * menangani restore transform
     */

    camera::
    NativeCameraController::
    instance()
    .disable();



    return true;

}







void FreecamController::forceDisable()
noexcept
{

    mModuleEnabled.store(
        false
    );


    mRequestedActive.store(
        false
    );



    camera::
    NativeCameraController::
    instance()
    .disable();



    PlayerFreezeController::
    instance()
    .disable();



    clearSessionState();

}







void FreecamController::clearSessionState()
noexcept
{

    mCurrentPlayer.store(
        nullptr
    );


}







void FreecamController::notePlayerAuthInput()
noexcept
{

    mPlayerAuthInputSeen.fetch_add(
        1
    );

}







bool FreecamController::moduleEnabled()
const noexcept
{

    return mModuleEnabled.load();

}







bool FreecamController::active()
const noexcept
{

    return mRequestedActive.load();

}







bool FreecamController::cameraActive()
const noexcept
{

    return camera::
        NativeCameraController::
        instance()
        .isEnabled();

}







bool FreecamController::shouldSuppressPlayerAuthInput()
const noexcept
{

    /*
     * Tidak lagi bergantung spectator.
     *
     * Untuk tahap awal native freecam:
     * packet tetap normal.
     *
     * Nanti bisa diganti dengan
     * input isolation layer.
     */

    return false;

}







std::uint64_t
FreecamController::playerAuthInputSeen()
const noexcept
{

    return
        mPlayerAuthInputSeen.load();

}


}
