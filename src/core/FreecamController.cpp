#include "core/FreecamController.hpp"

namespace levifreecam {

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

    /*
     * If master module is disabled,
     * Freecam must always be disabled too.
     */
    if (!enabled) {
        forceDisable();
    }
}

void FreecamController::setActive(
    bool active
) noexcept {

    /*
     * CAM cannot activate if the main
     * Freecam module is disabled.
     */
    if (
        active &&
        !moduleEnabled()
    ) {
        return;
    }

    mActive.store(
        active,
        std::memory_order_release
    );

    /*
     * When CAM is turned off,
     * spectator state must also become invalid.
     *
     * Later GameModeController will perform
     * the actual gamemode restoration.
     */
    if (!active) {
        mSpectatorApplied.store(
            false,
            std::memory_order_release
        );
    }
}

void FreecamController::forceDisable()
    noexcept {

    mActive.store(
        false,
        std::memory_order_release
    );

    mSpectatorApplied.store(
        false,
        std::memory_order_release
    );
}

void FreecamController::setSpectatorApplied(
    bool applied
) noexcept {

    mSpectatorApplied.store(
        applied,
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

    return mActive.load(
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
     * IMPORTANT:
     *
     * PlayerAuthInput is ONLY blocked when:
     *
     * 1. Freecam module is enabled
     * 2. CAM button is active
     * 3. Local spectator spoof succeeded
     *
     * Condition #3 is deliberately false
     * in v0.2.0.
     */
    return
        moduleEnabled() &&
        active() &&
        spectatorApplied();
}

std::uint64_t
FreecamController::playerAuthInputSeen()
    const noexcept {

    return mPlayerAuthInputSeen.load(
        std::memory_order_relaxed
    );
}

} // namespace levifreecam
