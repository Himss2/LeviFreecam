#include "core/PlayerFreezeController.hpp"

#include <android/log.h>


namespace levifreecam {


namespace {


constexpr char kLogTag[] =
    "Levi Freecam";


}



PlayerFreezeController&
PlayerFreezeController::instance()
noexcept
{

    static PlayerFreezeController controller;

    return controller;

}





void PlayerFreezeController::enable(
    void* player
)
noexcept
{

    if(player == nullptr)
        return;



    if(mEnabled.load())
        return;



    mPlayer.store(
        player
    );



    mCaptured.store(
        true
    );



    mEnabled.store(
        true
    );



    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "Freeze session captured"

    );

}





void PlayerFreezeController::tick(
    void* player
)
noexcept
{

    if(
        !mEnabled.load()
    )
        return;



    if(
        player == nullptr
    )
        return;



    /*
     *
     * TEMPORARY SAFE FREEZE
     *
     * Belum melakukan memory write.
     *
     * Karena offset Actor belum ditemukan.
     *
     *
     * Tahap berikut:
     *
     * Actor position overwrite
     * Velocity zero
     * Movement state lock
     *
     */



}





void PlayerFreezeController::disable()
noexcept
{


    if(
        !mEnabled.load()
    )
        return;



    mEnabled.store(
        false
    );


    mCaptured.store(
        false
    );


    mPlayer.store(
        nullptr
    );



    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "Freeze session released"

    );


}






bool PlayerFreezeController::enabled()
const noexcept
{

    return
        mEnabled.load();

}





void*
PlayerFreezeController::player()
const noexcept
{

    return
        mPlayer.load();

}



}
