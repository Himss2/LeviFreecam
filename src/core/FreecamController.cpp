#include "core/FreecamController.hpp"


#include "camera/NativeCameraController.hpp"
#include "game/GameModeController.hpp"


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
) noexcept
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


        clearSessionState();

    }

}





void FreecamController::setActive(
    bool active
) noexcept
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
            "Freecam requested ON"
        );


    }
    else
    {


        restoreNow();


        camera::
        NativeCameraController::
        instance()
        .disable();



        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Freecam requested OFF"
        );


    }


}






void FreecamController::onLocalPlayerTick(
    void* localPlayer
) noexcept
{


    if(localPlayer==nullptr)
        return;



    /*
     * simpan player
     */
    mCurrentPlayer.store(
        localPlayer
    );



    if(
        !active()
    )
    {
        return;
    }



    auto& gameMode =
        game::
        GameModeController::
        instance();



    /*
     * Resolve safety
     */
    if(
        !gameMode.available()
    )
    {

        __android_log_print(
            ANDROID_LOG_ERROR,
            kLogTag,
            "GameMode unavailable"
        );

        return;

    }




    /*
     * Apply spectator sekali
     */

    if(
        !mSpectatorApplied.load()
    )
    {


        auto current =
            gameMode.getLocalGameType(
                localPlayer
            );



        if(
            current.has_value()
        )
        {


            if(
                !mOriginalGameTypeValid.load()
            )
            {


                mOriginalGameType.store(
                    current.value()
                );


                mOriginalGameTypeValid.store(
                    true
                );


                __android_log_print(
                    ANDROID_LOG_INFO,
                    kLogTag,
                    "Saved GameType %d",
                    current.value()
                );

            }



            if(
                gameMode.setLocalGameType(
                    localPlayer,
                    game::GameType::Spectator
                )
            )
            {

                mSpectatorApplied.store(
                    true
                );


                __android_log_print(
                    ANDROID_LOG_INFO,
                    kLogTag,
                    "Spectator mode applied"
                );


            }


        }


    }




    /*
     * Refresh spectator
     *
     * agar Minecraft tidak overwrite
     */

    auto tick =
        mSpectatorRefreshTicks.fetch_add(
            1
        );



    if(
        tick > 40
    )
    {

        gameMode.setLocalGameType(
            localPlayer,
            game::GameType::Spectator
        );


        mSpectatorRefreshTicks.store(
            0
        );

    }





    camera::
    NativeCameraController::
    instance()
    .update();


}








bool FreecamController::restoreNow()
noexcept
{


    auto player =
        mCurrentPlayer.load();



    if(
        player==nullptr
    )
    {
        return false;
    }



    if(
        !mOriginalGameTypeValid.load()
    )
    {
        return false;
    }



    auto& gameMode =
        game::
        GameModeController::
        instance();



    bool result =
        gameMode.setLocalGameType(
            player,
            mOriginalGameType.load()
        );



    if(result)
    {


        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "GameType restored"
        );


        mSpectatorApplied.store(
            false
        );


        mOriginalGameTypeValid.store(
            false
        );


    }



    return result;

}






void FreecamController::forceDisable()
noexcept
{

    restoreNow();



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



    clearSessionState();

}






void FreecamController::clearSessionState()
noexcept
{


    mCurrentPlayer.store(
        nullptr
    );


    mSpectatorApplied.store(
        false
    );


    mOriginalGameTypeValid.store(
        false
    );


    mSpectatorRefreshTicks.store(
        0
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




bool FreecamController::spectatorApplied()
const noexcept
{
    return mSpectatorApplied.load();
}




bool FreecamController::shouldSuppressPlayerAuthInput()
const noexcept
{

    return
        spectatorApplied();

}




std::uint64_t
FreecamController::playerAuthInputSeen()
const noexcept
{

    return
        mPlayerAuthInputSeen.load();

}



}
