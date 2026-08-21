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
            "Freecam ON"
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
            "Freecam OFF"
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



    if(
        !active()
    )
    {
        return;
    }



    /*
     * FREECAM WITHOUT SPECTATOR
     *
     * Player tetap game mode asli.
     *
     * Kita hanya freeze entity.
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



    camera::
    NativeCameraController::
    instance()
    .update();


}




bool FreecamController::restoreNow()
noexcept
{

    /*
     * Tidak ada restore gamemode.
     *
     * Spectator sudah diputus.
     */


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



    PlayerFreezeController::
    instance()
    .disable();



    camera::
    NativeCameraController::
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




bool FreecamController::shouldSuppressPlayerAuthInput()
const noexcept
{

    /*
     * Sebelumnya:
     *
     * return spectatorApplied()
     *
     * Sekarang:
     *
     * Freecam aktif langsung block input.
     */


    return active();

}




std::uint64_t
FreecamController::playerAuthInputSeen()
const noexcept
{

    return mPlayerAuthInputSeen.load();

}


}
