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
    {
        return;
    }






    if(
        mEnabled.load()
    )
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
     * Jangan isi posisi dummy.
     *
     * Offset Actor belum final.
     *
     * Memory write akan masuk
     * setelah RE Actor selesai.
     */

    mPosition[0] = 0.0f;
    mPosition[1] = 0.0f;
    mPosition[2] = 0.0f;




    mRotation[0] = 0.0f;
    mRotation[1] = 0.0f;







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
     * PLACEHOLDER
     *
     * Native freeze belum melakukan
     * memory modification.
     *
     * Tahap berikut:
     *
     * Actor velocity lock
     * Actor position lock
     * Input lock
     *
     */



}









void PlayerFreezeController::disable()
noexcept
{

    if(
        !mEnabled.load()
    )
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

    return
        mEnabled.load();

}



}
