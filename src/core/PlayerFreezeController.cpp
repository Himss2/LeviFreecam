#include "core/PlayerFreezeController.hpp"

#include <android/log.h>


namespace levifreecam {


namespace {


constexpr char kLogTag[] =
    "LeviFreecam";


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

    if(
        player == nullptr
    )
    {
        return;
    }



    /*
     * Simpan pointer player
     */
    mPlayer.store(
        player
    );



    /*
     * Temporary

     * Nanti diganti:
     *
     * Actor::getPosition()
     * Actor::getRotation()
     *
     */


    mPosition[0] = 0.0f;
    mPosition[1] = 0.0f;
    mPosition[2] = 0.0f;



    mEnabled.store(
        true
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Player freeze enabled"
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
    {
        return;
    }



    if(
        player == nullptr
    )
    {
        return;
    }



    /*
     * TEMPORARY

     * Belum melakukan memory write.

     * Setelah Actor offset ditemukan:
     *
     * setPosition()
     * setVelocity()
     * resetFallDistance()
     *
     * akan masuk di sini.
     */


}







void PlayerFreezeController::disable()
noexcept
{

    mEnabled.store(
        false
    );



    mPlayer.store(
        nullptr
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Player freeze disabled"
    );

}







bool PlayerFreezeController::enabled()
const noexcept
{

    return mEnabled.load();

}


}
