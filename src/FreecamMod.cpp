#include "FreecamMod.hpp"


#include "core/FreecamController.hpp"

#include "camera/CameraHook.hpp"

#include "render/FirstPersonHook.hpp"


#include <pl/Mod.hpp>



namespace levifreecam {



FreecamMod&
FreecamMod::instance()
{

    static FreecamMod mod;

    return mod;

}







FreecamMod::FreecamMod()
    :
    mSelf(
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
        "Loading Levi Freecam Native v1.0"
    );


    return true;

}









bool FreecamMod::enable()
{

    auto& self =
        getSelf();





    /*
     * Player Hook
     */

    if(
        !mPlayerHook.install()
    )
    {

        self.getLogger().
        error(
            "Failed install Player hook"
        );


        return false;

    }





    /*
     * Packet Hook
     */

    if(
        !mPacketHook.install()
    )
    {

        self.getLogger().
        error(
            "Failed install Packet hook"
        );


        mPlayerHook.uninstall();


        return false;

    }







    /*
     * Camera Hook
     */

    if(
        !camera::installCameraHook()
    )
    {

        self.getLogger().
        error(
            "Camera hook failed"
        );


        mPacketHook.uninstall();

        mPlayerHook.uninstall();


        return false;

    }







    /*
     * First Person Hook
     */

    if(
        !render::FirstPersonHook::
        instance()
        .install()
    )
    {

        self.getLogger().
        error(
            "First person hook failed"
        );


        camera::removeCameraHook();


        mPacketHook.uninstall();

        mPlayerHook.uninstall();


        return false;

    }







    /*
     * Register Mod Menu
     */

    const bool registered =
        mModMenu.registerAll(

            self.getId(),



            [this](bool enabled)
            {

                setModuleEnabled(
                    enabled
                );

            },



            [this](bool active)
            {

                setCameraActive(
                    active
                );


                render::FirstPersonHook::
                instance()
                .setEnabled(
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
            "Failed register ModMenu"
        );



        render::FirstPersonHook::
        instance()
        .uninstall();



        camera::removeCameraHook();


        mPacketHook.uninstall();

        mPlayerHook.uninstall();


        return false;

    }







    self.getLogger().
    info(
        "Levi Native Freecam Ready"
    );



    return true;

}









bool FreecamMod::disable()
{

    restoreAndReset();





    render::FirstPersonHook::
    instance()
    .uninstall();





    camera::removeCameraHook();





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





    render::FirstPersonHook::
    instance()
    .uninstall();





    camera::removeCameraHook();





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

    FreecamController::
    instance()
    .setModuleEnabled(
        enabled
    );



    getSelf().
    getLogger().
    info(
        "Freecam module {}",
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
        "Camera {}",
        active
        ? "ON"
        : "OFF"
    );

}









void FreecamMod::restoreAndReset()
{

    auto& controller =
        FreecamController::
        instance();



    controller.restoreNow();



    controller.forceDisable();



}






}





PL_REGISTER_MOD(
    levifreecam::FreecamMod,
    levifreecam::FreecamMod::instance()
)
