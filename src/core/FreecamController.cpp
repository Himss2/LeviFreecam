#include "core/FreecamController.hpp"

#include "game/GameModeController.hpp"
#include "game/GameTypes.hpp"

#include <android/log.h>

#include <cstdint>

namespace levifreecam {

namespace {

constexpr char kLogTag[] =
    "Levi Freecam";

/*
 * Minecraft normal tick = sekitar 20 TPS.
 *
 * 4 tick sekitar 200 ms.
 *
 * Ambient juga terlihat melakukan
 * refresh spectator sekitar 200 ms.
 */
constexpr std::uint32_t
    kSpectatorRefreshIntervalTicks =
        4;

} // namespace


FreecamController&
FreecamController::instance() {

    static FreecamController controller;

    return controller;
}


void FreecamController::setModuleEnabled(
    bool enabled
) noexcept {

    mModuleEnabled.store(
        enabled,
        std::memory_order_release
    );

    if (!enabled) {

        /*
         * Jangan langsung menghapus
         * mSpectatorApplied.
         *
         * Kita masih harus memblokir
         * PlayerAuthInput selama gamemode
         * belum direstore.
         */
        mRequestedActive.store(
            false,
            std::memory_order_release
        );
    }
}


void FreecamController::setActive(
    bool active
) noexcept {

    /*
     * CAM tidak boleh ON apabila module
     * Freecam utama OFF.
     */
    if (
        active &&
        !moduleEnabled()
    ) {
        return;
    }

    /*
     * Callback tombol hanya meminta state.
     *
     * Kita tidak menyentuh LocalPlayer
     * langsung dari UI thread.
     */
    mRequestedActive.store(
        active,
        std::memory_order_release
    );
}


void FreecamController::onLocalPlayerTick(
    void* localPlayer
) noexcept {

    if (localPlayer == nullptr) {
        return;
    }

    auto& gameMode =
        game::GameModeController::
            instance();

    /*
     * Simpan LocalPlayer terbaru.
     */
    void* previousPlayer =
        mCurrentPlayer.exchange(

            localPlayer,

            std::memory_order_acq_rel
        );

    /*
     * Pointer berubah biasanya berarti:
     *
     * - masuk world
     * - pindah dimension
     * - player object dibuat ulang
     *
     * Jangan pernah dereference object lama.
     */
    if (
        previousPlayer != nullptr &&
        previousPlayer != localPlayer
    ) {

        mSpectatorApplied.store(
            false,
            std::memory_order_release
        );

        mOriginalGameTypeValid.store(
            false,
            std::memory_order_release
        );

        mSpectatorRefreshTicks.store(
            0,
            std::memory_order_release
        );
    }

    /*
     * Tidak ada GameType API.
     *
     * Tetap aman.
     * Packet tidak akan diblokir.
     */
    if (!gameMode.available()) {
        return;
    }

    const bool wantsFreecam =

        moduleEnabled() &&

        mRequestedActive.load(
            std::memory_order_acquire
        );


    /*
     * =====================================================
     * FREECAM ON
     * =====================================================
     */
    if (wantsFreecam) {

        /*
         * Belum spectator.
         *
         * Simpan original GameType.
         */
        if (!spectatorApplied()) {

            const auto originalGameType =
                gameMode.getLocalGameType(
                    localPlayer
                );

            if (
                !originalGameType.has_value()
            ) {
                return;
            }

            mOriginalGameType.store(

                *originalGameType,

                std::memory_order_release
            );

            mOriginalGameTypeValid.store(
                true,
                std::memory_order_release
            );

            /*
             * Terapkan spectator hanya ke
             * LocalPlayer.
             *
             * Tidak memberi tahu server.
             */
            if (
                !gameMode.setLocalGameType(

                    localPlayer,

                    game::GameType::Spectator
                )
            ) {

                mOriginalGameTypeValid.store(
                    false,
                    std::memory_order_release
                );

                return;
            }

            /*
             * PENTING:
             *
             * PlayerAuthInput baru boleh
             * diblokir setelah spectator
             * berhasil diterapkan.
             */
            mSpectatorApplied.store(
                true,
                std::memory_order_release
            );

            mSpectatorRefreshTicks.store(
                0,
                std::memory_order_release
            );

            __android_log_print(

                ANDROID_LOG_INFO,

                kLogTag,

                "Freecam ON: local spectator "
                "applied, original GameType=%d",

                *originalGameType
            );

            return;
        }

        /*
         * =================================================
         * REFRESH SPECTATOR
         * =================================================
         *
         * Server/game state kadang mencoba
         * mengembalikan gamemode client.
         *
         * Maka spectator kita apply kembali
         * setiap sekitar 200 ms.
         */
        const std::uint32_t refreshTicks =

            mSpectatorRefreshTicks.fetch_add(

                1,

                std::memory_order_acq_rel

            ) + 1;

        if (
            refreshTicks >=
            kSpectatorRefreshIntervalTicks
        ) {

            gameMode.setLocalGameType(

                localPlayer,

                game::GameType::Spectator
            );

            mSpectatorRefreshTicks.store(
                0,
                std::memory_order_release
            );
        }

        return;
    }


    /*
     * =====================================================
     * FREECAM OFF
     * =====================================================
     *
     * CAM sudah OFF tetapi local player
     * mungkin masih spectator.
     *
     * Jangan buka PlayerAuthInput sampai
     * original GameType berhasil direstore.
     */
    if (spectatorApplied()) {

        if (
            !mOriginalGameTypeValid.load(
                std::memory_order_acquire
            )
        ) {

            /*
             * Kita tidak tahu mode original.
             *
             * Lebih aman tetap suppress packet
             * daripada mengirim posisi Freecam
             * ke server.
             */
            return;
        }

        const std::int32_t
            originalGameType =

                mOriginalGameType.load(
                    std::memory_order_acquire
                );

        /*
         * Restore original client GameType.
         */
        if (
            !gameMode.setLocalGameType(

                localPlayer,

                originalGameType
            )
        ) {
            return;
        }

        /*
         * Sekarang packet movement normal
         * boleh dikirim lagi.
         */
        mSpectatorApplied.store(
            false,
            std::memory_order_release
        );

        mOriginalGameTypeValid.store(
            false,
            std::memory_order_release
        );

        mSpectatorRefreshTicks.store(
            0,
            std::memory_order_release
        );

        __android_log_print(

            ANDROID_LOG_INFO,

            kLogTag,

            "Freecam OFF: restored "
            "local GameType=%d",

            originalGameType
        );
    }
}


bool FreecamController::restoreNow()
    noexcept {

    /*
     * Belum pernah masuk spectator.
     */
    if (!spectatorApplied()) {
        return true;
    }

    auto& gameMode =
        game::GameModeController::
            instance();

    void* localPlayer =
        mCurrentPlayer.load(
            std::memory_order_acquire
        );

    if (
        !gameMode.available() ||
        localPlayer == nullptr ||
        !mOriginalGameTypeValid.load(
            std::memory_order_acquire
        )
    ) {
        return false;
    }

    const std::int32_t
        originalGameType =

            mOriginalGameType.load(
                std::memory_order_acquire
            );

    if (
        !gameMode.setLocalGameType(

            localPlayer,

            originalGameType
        )
    ) {
        return false;
    }

    mSpectatorApplied.store(
        false,
        std::memory_order_release
    );

    mOriginalGameTypeValid.store(
        false,
        std::memory_order_release
    );

    mSpectatorRefreshTicks.store(
        0,
        std::memory_order_release
    );

    return true;
}


void FreecamController::forceDisable()
    noexcept {

    mModuleEnabled.store(
        false,
        std::memory_order_release
    );

    mRequestedActive.store(
        false,
        std::memory_order_release
    );

    clearSessionState();
}


void FreecamController::clearSessionState()
    noexcept {

    mSpectatorApplied.store(
        false,
        std::memory_order_release
    );

    mOriginalGameTypeValid.store(
        false,
        std::memory_order_release
    );

    mOriginalGameType.store(
        0,
        std::memory_order_release
    );

    mCurrentPlayer.store(
        nullptr,
        std::memory_order_release
    );

    mSpectatorRefreshTicks.store(
        0,
        std::memory_order_release
    );
}


void FreecamController::notePlayerAuthInput()
    noexcept {

    mPlayerAuthInputSeen.fetch_add(

        1,

        std::memory_order_relaxed
    );
}


bool FreecamController::moduleEnabled()
    const noexcept {

    return mModuleEnabled.load(
        std::memory_order_acquire
    );
}


bool FreecamController::active()
    const noexcept {

    return mRequestedActive.load(
        std::memory_order_acquire
    );
}


bool FreecamController::spectatorApplied()
    const noexcept {

    return mSpectatorApplied.load(
        std::memory_order_acquire
    );
}


bool
FreecamController::shouldSuppressPlayerAuthInput()
    const noexcept {

    /*
     * Jangan bergantung pada state tombol.
     *
     * Ketika CAM ditekan OFF ada periode
     * singkat ketika:
     *
     * requestedActive = false
     * spectatorApplied = true
     *
     * Pada periode itu PlayerAuthInput masih
     * WAJIB diblokir sampai restore selesai.
     */
    return spectatorApplied();
}


std::uint64_t
FreecamController::playerAuthInputSeen()
    const noexcept {

    return mPlayerAuthInputSeen.load(
        std::memory_order_relaxed
    );
}

} // namespace levifreecam
