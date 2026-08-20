#include "FreecamMod.hpp"

#include "core/FreecamController.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {

FreecamMod &FreecamMod::instance() {
    static FreecamMod mod;
    return mod;
}

FreecamMod::FreecamMod() : mSelf(*ll::mod::NativeMod::current()) {}

ll::mod::NativeMod &FreecamMod::getSelf() const noexcept {
    return mSelf;
}

bool FreecamMod::load() {
    auto &self = getSelf();
    self.getLogger().info("Loading Levi Freecam v0.1.0");
    self.getLogger().info("Foundation build: no Minecraft hooks are installed yet");
    return true;
}

bool FreecamMod::enable() {
    auto &self = getSelf();

    const bool registered = mModMenu.registerAll(
        self.getId(),
        [this](bool enabled) { setFeatureEnabled(enabled); });

    if (!registered) {
        self.getLogger().error("Failed to register Freecam module/button in Mod Menu");
        return false;
    }

    self.getLogger().info("Freecam Mod Menu integration enabled");
    return true;
}

bool FreecamMod::disable() {
    auto &self = getSelf();

    // This call is already important in v0.1.0 and becomes safety-critical
    // once spectator spoof and packet suppression are added.
    FreecamController::instance().forceDisable();
    mModMenu.unregisterAll();

    self.getLogger().info("Levi Freecam disabled");
    return true;
}

bool FreecamMod::unload() {
    FreecamController::instance().forceDisable();
    mModMenu.unregisterAll();
    getSelf().getLogger().info("Levi Freecam unloaded");
    return true;
}

void FreecamMod::setFeatureEnabled(bool enabled) {
    auto &controller = FreecamController::instance();
    controller.setEnabled(enabled);

    getSelf().getLogger().info(
        "Freecam state = {} (v0.1.0 state-only; gameplay hook not active yet)",
        enabled ? "ON" : "OFF");
}

} // namespace levifreecam

PL_REGISTER_MOD(levifreecam::FreecamMod, levifreecam::FreecamMod::instance())
