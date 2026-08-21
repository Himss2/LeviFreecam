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

    if(player == nullptr)
    {
        return;
    }



    /*
     * Jangan enable berulang.
     *
     * Sebelumnya fungsi ini dipanggil
     * setiap tick sehingga log spam.
     */
    if(mEnabled.load())
    {
        return;
    }



    /*
     * Simpan LocalPlayer
     */
    mPlayer.store(
        player
    );



    /*
     * Backup posisi sementara.
     *
     * Nanti diganti dengan:
     *
     * Actor::getPosition()
     * Actor::getRotation()
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



    if(player == nullptr)
    {
        return;
    }



    /*
     * TEMPORARY
     *
     * Belum melakukan memory write.
     *
     * Nanti masuk:
     *
     * Actor::setPosition()
     * Actor::setVelocity()
     * resetFallDistance()
     *
     */

}






void PlayerFreezeController::disable()
noexcept
{

    /*
     * Jangan disable ulang
     */
    if(!mEnabled.load())
    {
        return;
    }



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
