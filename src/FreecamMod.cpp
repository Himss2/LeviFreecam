#include "FreecamMod.hpp"

#include "core/FreecamController.hpp"
#include "game/GameModeController.hpp"
#include "camera/CameraSystemHook.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {


FreecamMod&
FreecamMod::instance() {

    static FreecamMod mod;

    return mod;
}


FreecamMod::FreecamMod()
    : mSelf(
        *ll::mod::NativeMod::current()
    ) {
}


ll::mod::NativeMod&
FreecamMod::getSelf()
    const noexcept {

    return mSelf;
}


bool FreecamMod::load() {

    getSelf().
        getLogger().
        info(
            "Loading Levi Freecam v0.4.0"
        );

    return true;
}


bool FreecamMod::enable() {

    auto& self =
        getSelf();

    auto& gameMode =
        game::GameModeController::
            instance();

    /*
     * =====================================================
     * RESOLVE GAMETYPE FUNCTIONS
     * =====================================================
     *
     * These signatures were generated directly from
     * the supplied libminecraftpe.so.
     */
    if (!gameMode.resolve()) {

        self.getLogger().error(
            "Failed to resolve local GameType functions "
            "for this Minecraft build."
        );

        return false;
    }

    self.getLogger().info(
        "GameType getter resolved at 0x{:x}",
        gameMode.getterAddress()
    );

    self.getLogger().info(
        "Local GameType setter resolved at 0x{:x}",
        gameMode.setterAddress()
    );

    /*
     * Expected for the supplied binary:
     *
     * getter:
     *     0x0F0CB1B4
     *
     * setter:
     *     0x0A649B7C
     */

    /*
     * =====================================================
     * LOCAL PLAYER NORMAL TICK
     * =====================================================
     */
    if (!mPlayerHook.install()) {

        self.getLogger().error(
            "Failed to install "
            "LocalPlayer::normalTick hook."
        );

        gameMode.clear();

        return false;
    }

    self.getLogger().info(
        "LocalPlayer::normalTick hook installed "
        "at 0x{:x}",
        mPlayerHook.targetAddress()
    );

    /*
     * Expected for supplied binary:
     *
     * 0x0A6417E4
     */

    /*
     * =====================================================
     * OUTGOING PACKET HOOK
     * =====================================================
     */
    if (!mPacketHook.install()) {

        self.getLogger().error(
            "Failed to install "
            "LoopbackPacketSender::sendToServer hook."
        );

        mPlayerHook.uninstall();

        gameMode.clear();

        return false;
    }

    self.getLogger().info(
        "sendToServer hook installed "
        "at 0x{:x}",
        mPacketHook.targetAddress()
    );

    /*
     * Expected for supplied binary:
     *
     * 0x0C2E2AA8
     */

    /*
     * =====================================================
     * LEVI MOD MENU
     * =====================================================
     */
    const bool registered =
        mModMenu.registerAll(

            self.getId(),

            [this](
                bool enabled
            ) {
                setModuleEnabled(
                    enabled
                );
            },

            [this](
                bool active
            ) {
                setCameraActive(
                    active
                );
            }
        );

    if (!registered) {

        self.getLogger().error(
            "Failed to register "
            "Freecam module/CAM button."
        );

        mPacketHook.uninstall();

        mPlayerHook.uninstall();

        gameMode.clear();

        return false;
    }

    self.getLogger().info(
        "Levi Freecam v0.4.0 ready."
    );

    self.getLogger().info(
        "CAM will activate only after "
        "PlayerAuthInput detection is confirmed."
    );

    return true;
}

   if(!camera::installCameraSystemHook()){
       self.getLogger().error(
           "Camera system hook failed"
       );

       return false;
}

bool FreecamMod::disable() {

    restoreAndReset();

    mModMenu.unregisterAll();

    mPacketHook.uninstall();

    mPlayerHook.uninstall();

    game::GameModeController::
        instance().
        clear();

    getSelf().
        getLogger().
        info(
            "Levi Freecam disabled"
        );

    return true;
}


bool FreecamMod::unload() {

    restoreAndReset();

    mModMenu.unregisterAll();

    mPacketHook.uninstall();

    mPlayerHook.uninstall();

    game::GameModeController::
        instance().
        clear();

    getSelf().
        getLogger().
        info(
            "Levi Freecam unloaded"
        );

    return true;
}


void FreecamMod::setModuleEnabled(
    bool enabled
) {

    auto& controller =
        FreecamController::
            instance();

    controller.setModuleEnabled(
        enabled
    );

    getSelf().
        getLogger().
        info(
            "Freecam module = {}",
            enabled
                ? "ON"
                : "OFF"
        );
}


void FreecamMod::setCameraActive(
    bool active
) {

    auto& controller =
        FreecamController::
            instance();

    controller.setActive(
        active
    );

    getSelf().
        getLogger().
        info(
            "CAM requested = {}",
            controller.active()
                ? "ON"
                : "OFF"
        );

    getSelf().
        getLogger().
        info(
            "PlayerAuthInput seen = {}",
            controller.playerAuthInputSeen()
        );
}


void FreecamMod::restoreAndReset() {

    auto& controller =
        FreecamController::
            instance();

    /*
     * Request feature OFF.
     */
    controller.setModuleEnabled(
        false
    );

    /*
     * Try to restore original local GameType
     * while the GameType resolver and player
     * object are still available.
     */
    if (
        !controller.restoreNow() &&
        controller.spectatorApplied()
    ) {

        getSelf().
            getLogger().
            warn(
                "Could not restore local GameType "
                "during mod shutdown."
            );
    }

    controller.forceDisable();
}

} // namespace levifreecam


PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
