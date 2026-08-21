#include "FreecamMod.hpp"

#include "core/FreecamController.hpp"
#include "camera/CameraSystemHook.hpp"

#include <pl/Mod.hpp>


namespace levifreecam {


FreecamMod&
FreecamMod::instance()
{

    static FreecamMod mod;

    return mod;
}



FreecamMod::FreecamMod()
    : mSelf(
        *ll::mod::NativeMod::current()
    )
{
}



ll::mod::NativeMod&
FreecamMod::getSelf()
    const noexcept
{

    return mSelf;

}







bool FreecamMod::load()
{

    getSelf().
        getLogger().
        info(
            "Loading Levi Freecam v0.6.1 (No Spectator)"
        );


    return true;

}









bool FreecamMod::enable()
{

    auto& self =
        getSelf();



    /*
     * =====================================================
     * PLAYER TICK HOOK
     * =====================================================
     *
     * Tidak ada lagi GameMode resolve.
     *
     * Freecam sekarang berjalan:
     *
     * Camera takeover
     * +
     * Player freeze
     * +
     * Packet suppression
     *
     */


    if(
        !mPlayerHook.install()
    )
    {


        self.getLogger().
            error(
                "Failed install LocalPlayer hook."
            );


        return false;

    }



    self.getLogger().
        info(
            "Player hook installed 0x{:x}",
            mPlayerHook.targetAddress()
        );









    /*
     * =====================================================
     * PACKET HOOK
     * =====================================================
     */


    if(
        !mPacketHook.install()
    )
    {


        self.getLogger().
            error(
                "Failed install Packet hook."
            );



        mPlayerHook.uninstall();



        return false;

    }




    self.getLogger().
        info(
            "Packet hook installed 0x{:x}",
            mPacketHook.targetAddress()
        );









    /*
     * =====================================================
     * CAMERA HOOK
     * =====================================================
     */


    if(
        !camera::installCameraSystemHook()
    )
    {


        self.getLogger().
            error(
                "Camera hook failed."
            );



        mPacketHook.uninstall();


        mPlayerHook.uninstall();



        return false;

    }





    self.getLogger().
        info(
            "Camera hook installed 0x{:x}",
            camera::cameraSystemAddress()
        );









    /*
     * =====================================================
     * MOD MENU
     * =====================================================
     */


    const bool registered =
        mModMenu.registerAll(

            self.getId(),


            [this](
                bool enabled
            )
            {

                setModuleEnabled(
                    enabled
                );

            },



            [this](
                bool active
            )
            {

                setCameraActive(
                    active
                );

            }

        );






    if(
        !registered
    )
    {


        self.getLogger().
            error(
                "Failed register ModMenu."
            );



        camera::removeCameraSystemHook();


        mPacketHook.uninstall();


        mPlayerHook.uninstall();



        return false;

    }






    self.getLogger().
        info(
            "Levi Freecam ready (Spectator removed)."
        );



    return true;

}









bool FreecamMod::disable()
{


    /*
     * Matikan state dahulu
     */

    restoreAndReset();



    camera::removeCameraSystemHook();



    mModMenu.unregisterAll();



    mPacketHook.uninstall();



    mPlayerHook.uninstall();





    getSelf().
        getLogger().
        info(
            "Levi Freecam disabled"
        );



    return true;

}









bool FreecamMod::unload()
{


    restoreAndReset();



    camera::removeCameraSystemHook();



    mModMenu.unregisterAll();



    mPacketHook.uninstall();



    mPlayerHook.uninstall();





    getSelf().
        getLogger().
        info(
            "Levi Freecam unloaded"
        );



    return true;

}









void FreecamMod::setModuleEnabled(
    bool enabled
)
{


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
)
{


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









void FreecamMod::restoreAndReset()
{


    auto& controller =
        FreecamController::instance();



    /*
     * Tidak ada restore GameType.
     *
     * Spectator sudah diputus.
     *
     * Restore hanya:
     *
     * - camera
     * - player freeze
     * - internal state
     */


    controller.restoreNow();



    controller.forceDisable();



}



} // namespace levifreecam





PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
