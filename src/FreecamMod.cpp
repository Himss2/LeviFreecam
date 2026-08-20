#include "FreecamMod.hpp"

#include "core/FreecamController.hpp"
#include "game/GameModeController.hpp"

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
            "Loading Levi Freecam v0.3.0"
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
     * RESOLVE LOCAL GAMEMODE API
     * =====================================================
     */
    const bool gameModeReady =
        gameMode.resolve();


    if (gameModeReady) {

        self.getLogger().info(

            "Local GameType API resolved: "
            "getter=0x{:x}, setter=0x{:x}",

            gameMode.getterAddress(),

            gameMode.setterAddress()
        );

    } else {

        /*
         * Jangan crash apabila symbol tidak
         * diekspor pada Minecraft tertentu.
         *
         * CAM tetap aman tetapi belum akan
         * mengaktifkan spectator.
         */
        self.getLogger().error(

            "Could not resolve "
            "LocalPlayer::"
            "setPlayerGameTypeWithoutServerNotification "
            "or Player::getPlayerGameType. "
            "CAM will remain safe but cannot "
            "enter spectator until a signature "
            "fallback is added for this "
            "Minecraft build."
        );
    }


    /*
     * =====================================================
     * LOCAL PLAYER TICK HOOK
     * =====================================================
     */
    if (!mPlayerHook.install()) {

        self.getLogger().error(

            "Failed to install "
            "LocalPlayer::normalTick hook. "
            "The NormalTick signature may not "
            "match this Minecraft build."
        );

        gameMode.clear();

        return false;
    }


    self.getLogger().info(

        "LocalPlayer normalTick hook "
        "installed at 0x{:x}",

        mPlayerHook.targetAddress()
    );


    /*
     * =====================================================
     * PACKET HOOK
     * =====================================================
     */
    if (!mPacketHook.install()) {

        self.getLogger().error(

            "Failed to install "
            "LoopbackPacketSender::sendToServer "
            "hook. The signature may not match "
            "this Minecraft build."
        );

        mPlayerHook.uninstall();

        gameMode.clear();

        return false;
    }


    self.getLogger().info(

        "sendToServer hook "
        "installed at 0x{:x}",

        mPacketHook.targetAddress()
    );


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
            "Freecam module/CAM button"
        );

        mPacketHook.uninstall();

        mPlayerHook.uninstall();

        gameMode.clear();

        return false;
    }


    self.getLogger().info(

        "Freecam v0.3.0 ready. "
        "CAM now requests local spectator "
        "and blocks PlayerAuthInput only "
        "after spectator is applied."
    );


    return true;
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
}


void FreecamMod::restoreAndReset() {

    auto& controller =
        FreecamController::
            instance();


    /*
     * Minta OFF dahulu.
     */
    controller.setModuleEnabled(
        false
    );


    /*
     * Saat seluruh native mod dimatikan,
     * PlayerHook sebentar lagi akan dilepas.
     *
     * Maka kita melakukan restore terakhir
     * sebelum hook dihapus.
     */
    if (
        !controller.restoreNow() &&
        controller.spectatorApplied()
    ) {

        getSelf().
            getLogger().
            warn(

                "Could not restore local "
                "GameType during native mod "
                "shutdown. The game may correct "
                "the local state when the "
                "player/world reloads."
            );
    }


    controller.forceDisable();
}

} // namespace levifreecam


PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
