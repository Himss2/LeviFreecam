#include "FreecamMod.hpp"

#include "core/FreecamController.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {

FreecamMod& FreecamMod::instance() {
    static FreecamMod mod;
    return mod;
}

FreecamMod::FreecamMod()
    : mSelf(*ll::mod::NativeMod::current()) {
}

ll::mod::NativeMod&
FreecamMod::getSelf() const noexcept {
    return mSelf;
}

bool FreecamMod::load() {
    getSelf().getLogger().info(
        "Loading Levi Freecam v0.2.0"
    );

    return true;
}

bool FreecamMod::enable() {
    auto& self = getSelf();

    /*
     * ---------------------------------------------------------
     * Install Minecraft network hook.
     * ---------------------------------------------------------
     *
     * Target:
     *
     * LoopbackPacketSender::sendToServer(Packet&)
     *
     */
    if (!mPacketHook.install()) {
        self.getLogger().error(
            "Failed to install "
            "LoopbackPacketSender::sendToServer hook. "
            "The signature may not match this Minecraft build."
        );

        return false;
    }

    self.getLogger().info(
        "sendToServer hook installed at 0x{:x}",
        mPacketHook.targetAddress()
    );

    /*
     * ---------------------------------------------------------
     * Register Levi Mod Menu + CAM button.
     * ---------------------------------------------------------
     */
    const bool registered =
        mModMenu.registerAll(
            self.getId(),

            [this](bool enabled) {
                setModuleEnabled(enabled);
            },

            [this](bool active) {
                setCameraActive(active);
            }
        );

    if (!registered) {
        self.getLogger().error(
            "Failed to register Freecam module/CAM button"
        );

        mPacketHook.uninstall();

        return false;
    }

    self.getLogger().info(
        "Freecam hook foundation ready. "
        "Enable Freecam in Mod Menu to show CAM."
    );

    return true;
}

bool FreecamMod::disable() {
    auto& controller =
        FreecamController::instance();

    /*
     * Always reset feature state BEFORE removing hooks.
     *
     * This becomes extremely important once the spectator
     * spoof is implemented.
     */
    controller.setModuleEnabled(false);
    controller.forceDisable();

    mModMenu.unregisterAll();

    mPacketHook.uninstall();

    getSelf().getLogger().info(
        "Levi Freecam disabled"
    );

    return true;
}

bool FreecamMod::unload() {
    auto& controller =
        FreecamController::instance();

    controller.setModuleEnabled(false);
    controller.forceDisable();

    mModMenu.unregisterAll();

    mPacketHook.uninstall();

    getSelf().getLogger().info(
        "Levi Freecam unloaded"
    );

    return true;
}

void FreecamMod::setModuleEnabled(
    bool enabled
) {
    auto& controller =
        FreecamController::instance();

    controller.setModuleEnabled(enabled);

    getSelf().getLogger().info(
        "Freecam module = {}",
        enabled ? "ON" : "OFF"
    );
}

void FreecamMod::setCameraActive(
    bool active
) {
    auto& controller =
        FreecamController::instance();

    controller.setActive(active);

    getSelf().getLogger().info(
        "CAM = {} "
        "(hook active, spectator spoof pending)",
        controller.active()
            ? "ON"
            : "OFF"
    );
}

} // namespace levifreecam


PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
