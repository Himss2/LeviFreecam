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
 * Minecraft normally runs game logic
 * around 20 ticks per second.
 *
 * Four ticks is approximately 200 ms.
 *
 * Ambient Freecam also periodically
 * reasserts its local spectator state.
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
         * Request Freecam OFF.
         *
         * Do NOT clear spectatorApplied here.
         *
         * PlayerAuthInput must remain blocked
         * until the original GameType has been
         * restored by onLocalPlayerTick().
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
     * CAM cannot be activated while the
     * Freecam module itself is disabled.
     */
    if (
        active &&
        !moduleEnabled()
    ) {
        return;
    }

    /*
     * Only request a state change here.
     *
     * Minecraft player functions are called
     * later from LocalPlayer::normalTick.
     */
    mRequestedActive.store(
        active,
        std::memory_order_release
    );

    if (!active) {
        mWaitingForPacketHookLogged.store(
            false,
            std::memory_order_release
        );
    }
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
     * Track current LocalPlayer object.
     */
    void* previousPlayer =
        mCurrentPlayer.exchange(
            localPlayer,
            std::memory_order_acq_rel
        );

    /*
     * LocalPlayer changed.
     *
     * This can happen when:
     *
     * - entering a world
     * - changing dimension
     * - player object recreation
     *
     * Never use state belonging to the old object.
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

        mWaitingForPacketHookLogged.store(
            false,
            std::memory_order_release
        );
    }

    /*
     * GameType functions must be resolved.
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
     * ======================================================
     * FREECAM ON
     * ======================================================
     */
    if (wantsFreecam) {

        /*
         * ==================================================
         * SAFETY CHECK
         * ==================================================
         *
         * Before entering local spectator we require proof
         * that PacketHook has successfully identified at
         * least one PlayerAuthInput packet.
         *
         * Otherwise there would be a risk that spectator
         * movement gets sent to the server.
         */
        if (
            !spectatorApplied() &&
            playerAuthInputSeen() == 0
        ) {

            bool expected =
                false;

            if (
                mWaitingForPacketHookLogged.
                    compare_exchange_strong(
                        expected,
                        true,
                        std::memory_order_acq_rel
                    )
            ) {

                __android_log_print(
                    ANDROID_LOG_WARN,
                    kLogTag,
                    "CAM waiting for "
                    "PlayerAuthInput validation"
                );
            }

            return;
        }

        /*
         * ==================================================
         * FIRST ACTIVATION
         * ==================================================
         */
        if (!spectatorApplied()) {

            /*
             * Read original GameType.
             */
            const auto originalGameType =
                gameMode.getLocalGameType(
                    localPlayer
                );

            if (
                !originalGameType.has_value()
            ) {

                __android_log_print(
                    ANDROID_LOG_ERROR,
                    kLogTag,
                    "Failed to read original GameType"
                );

                return;
            }

            /*
             * Save original GameType.
             */
            mOriginalGameType.store(
                *originalGameType,
                std::memory_order_release
            );

            mOriginalGameTypeValid.store(
                true,
                std::memory_order_release
            );

            /*
             * ==================================================
             * LOCAL SPECTATOR
             * ==================================================
             *
             * GameType::Spectator = 6.
             *
             * This calls the exact local client GameType
             * routine resolved from the supplied
             * libminecraftpe.so.
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

                __android_log_print(
                    ANDROID_LOG_ERROR,
                    kLogTag,
                    "Failed to apply local spectator"
                );

                return;
            }

            /*
             * IMPORTANT:
             *
             * Only after local spectator has been applied
             * may PacketHook begin dropping PlayerAuthInput.
             */
            mSpectatorApplied.store(
                true,
                std::memory_order_release
            );

            mSpectatorRefreshTicks.store(
                0,
                std::memory_order_release
            );

            mWaitingForPacketHookLogged.store(
                false,
                std::memory_order_release
            );

            __android_log_print(
                ANDROID_LOG_INFO,
                kLogTag,
                "Freecam ON: local spectator applied, "
                "original GameType=%d",
                *originalGameType
            );

            return;
        }

        /*
         * ==================================================
         * SPECTATOR REFRESH
         * ==================================================
         *
         * The server or game may occasionally overwrite
         * local client state.
         *
         * Reapply spectator approximately every 200 ms.
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
     * ======================================================
     * FREECAM OFF
     * ======================================================
     *
     * CAM has been turned OFF.
     *
     * PlayerAuthInput remains blocked until the original
     * local GameType is restored.
     */
    if (spectatorApplied()) {

        if (
            !mOriginalGameTypeValid.load(
                std::memory_order_acquire
            )
        ) {

            /*
             * We cannot safely restore an unknown mode.
             *
             * Keep packet suppression active instead of
             * sending the detached camera position.
             */
            return;
        }

        const std::int32_t
            originalGameType =
                mOriginalGameType.load(
                    std::memory_order_acquire
                );

        /*
         * Restore original client-side GameType.
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
         * Restoration succeeded.
         *
         * PlayerAuthInput may now flow normally.
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
            "Freecam OFF: restored local GameType=%d",
            originalGameType
        );
    }
}


bool FreecamController::restoreNow()
    noexcept {

    /*
     * Nothing to restore.
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

    mWaitingForPacketHookLogged.store(
        false,
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
     * Depend on the actual spectator state,
     * not merely the CAM button state.
     *
     * CAM OFF:
     *
     * requestedActive = false
     *
     * but spectatorApplied may still be true
     * until restoration finishes.
     *
     * During that window packets must remain blocked.
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
