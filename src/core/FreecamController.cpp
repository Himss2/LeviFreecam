#include "core/FreecamController.hpp"

#include "camera/NativeCameraController.hpp"

#include <android/log.h>

#include <cstdint>


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
        enabled,
        std::memory_order_release
    );


    if (!enabled)
    {
        mRequestedActive.store(
            false,
            std::memory_order_release
        );


        /*
         * Matikan native camera.
         */
        camera::
        NativeCameraController::
        instance()
        .disable();
    }
}




void FreecamController::setActive(
    bool active
) noexcept
{

    if (
        active &&
        !moduleEnabled()
    )
    {
        return;
    }


    mRequestedActive.store(
        active,
        std::memory_order_release
    );


    if (active)
    {

        if (
            camera::
            NativeCameraController::
            instance()
            .enable()
        )
        {

            __android_log_print(
                ANDROID_LOG_INFO,
                kLogTag,
                "Native Freecam enabled"
            );

        }
        else
        {

            __android_log_print(
                ANDROID_LOG_ERROR,
                kLogTag,
                "Native Freecam failed"
            );

        }

    }
    else
    {

        camera::
        NativeCameraController::
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
) noexcept
{

    if(localPlayer == nullptr)
    {
        return;
    }


    /*
     * Simpan player reference.
     *
     * Tidak lagi digunakan untuk
     * GameType.
     */
    mCurrentPlayer.store(
        localPlayer,
        std::memory_order_release
    );


    if(
        !moduleEnabled()
    )
    {
        return;
    }


    if(
        !active()
    )
    {
        return;
    }


    /*
     * Update native camera.
     *
     * Player tetap normal.
     */
    camera::
    NativeCameraController::
    instance()
    .update();

}




bool FreecamController::restoreNow()
    noexcept
{

    return camera::
    NativeCameraController::
    instance()
    .disable();

}




void FreecamController::forceDisable()
    noexcept
{

    mModuleEnabled.store(
        false,
        std::memory_order_release
    );


    mRequestedActive.store(
        false,
        std::memory_order_release
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
        nullptr,
        std::memory_order_release
    );


    mPlayerAuthInputSeen.store(
        0,
        std::memory_order_release
    );


}




void FreecamController::notePlayerAuthInput()
    noexcept
{

    /*
     * Tetap dipertahankan agar
     * kompatibel dengan PacketHook lama.
     *
     * Tidak lagi dipakai untuk block.
     */

    mPlayerAuthInputSeen.fetch_add(
        1,
        std::memory_order_relaxed
    );

}




bool FreecamController::moduleEnabled()
const noexcept
{

    return mModuleEnabled.load(
        std::memory_order_acquire
    );

}




bool FreecamController::active()
const noexcept
{

    return mRequestedActive.load(
        std::memory_order_acquire
    );

}




bool FreecamController::spectatorApplied()
const noexcept
{

    /*
     * Compatibility.
     *
     * Spectator sudah tidak digunakan.
     */

    return false;

}




bool FreecamController::shouldSuppressPlayerAuthInput()
const noexcept
{

    /*
     * Native camera tidak mengubah
     * posisi player.

     * Packet tidak perlu diblok.
     */

    return false;

}




std::uint64_t
FreecamController::playerAuthInputSeen()
const noexcept
{

    return mPlayerAuthInputSeen.load(
        std::memory_order_relaxed
    );

}



} // namespace levifreecam
