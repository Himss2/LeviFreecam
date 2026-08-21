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
     */
    if (!gameMode.resolve()) {

        self.getLogger().error(
            "Failed to resolve local GameType functions."
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
     * =====================================================
     * LOCAL PLAYER NORMAL TICK
     * =====================================================
     */
    if (!mPlayerHook.install()) {

        self.getLogger().error(
            "Failed to install LocalPlayer normalTick hook."
        );


        gameMode.clear();

        return false;
    }



    self.getLogger().info(
        "LocalPlayer hook installed at 0x{:x}",
        mPlayerHook.targetAddress()
    );



    /*
     * =====================================================
     * PACKET HOOK
     * =====================================================
     */
    if (!mPacketHook.install()) {


        self.getLogger().error(
            "Failed to install sendToServer hook."
        );


        mPlayerHook.uninstall();

        gameMode.clear();

        return false;
    }



    self.getLogger().info(
        "Packet hook installed at 0x{:x}",
        mPacketHook.targetAddress()
    );



    /*
     * =====================================================
     * CAMERA SYSTEM HOOK
     * =====================================================
     */
    if (!camera::installCameraSystemHook()) {


        self.getLogger().error(
            "Camera system hook failed."
        );


        mPacketHook.uninstall();

        mPlayerHook.uninstall();

        gameMode.clear();


        return false;
    }



    self.getLogger().info(
        "Camera system hook installed at 0x{:x}",
        camera::cameraSystemAddress()
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
            ){

                setModuleEnabled(
                    enabled
                );

            },


            [this](
                bool active
            ){

                setCameraActive(
                    active
                );

            }

        );



    if (!registered) {


        self.getLogger().error(
            "Failed to register Freecam module."
        );


        camera::removeCameraSystemHook();

        mPacketHook.uninstall();

        mPlayerHook.uninstall();

        gameMode.clear();


        return false;
    }



    self.getLogger().info(
        "Levi Freecam ready."
    );


    return true;
}



bool FreecamMod::disable() {


    camera::removeCameraSystemHook();


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


    camera::removeCameraSystemHook();


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



    controller.setModuleEnabled(
        false
    );



    if (
        !controller.restoreNow() &&
        controller.spectatorApplied()
    ) {


        getSelf().
            getLogger().
            warn(
                "Could not restore GameType."
            );

    }



    controller.forceDisable();

}


} // namespace levifreecam



PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
