#include "FreecamMod.hpp"


#include "core/FreecamController.hpp"


#include "camera/CameraHook.hpp"
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


    getSelf()
    .getLogger()
    .info(

        "Loading Levi Freecam Native v1.0"

    );



    return true;


}









bool FreecamMod::enable()

{


    auto& self =
        getSelf();





    /*
     *
     * Player Hook
     *
     */


    if(
        !mPlayerHook.install()
    )

    {


        self.getLogger()
        .error(

            "Failed install Player hook"

        );


        return false;


    }







    self.getLogger()
    .info(

        "Player hook installed 0x{:x}",

        mPlayerHook.targetAddress()

    );









    /*
     *
     * Packet Hook
     *
     */


    if(
        !mPacketHook.install()
    )

    {


        self.getLogger()
        .error(

            "Failed install Packet hook"

        );


        mPlayerHook.uninstall();


        return false;


    }









    /*
     *
     * Native Camera Hook
     *
     */


    if(
        !camera::installCameraHook()
    )

    {


        self.getLogger()
        .error(

            "Camera hook failed"

        );



        mPacketHook.uninstall();


        mPlayerHook.uninstall();



        return false;


    }







    self.getLogger()
    .info(

        "Camera hook installed 0x{:x}",

        camera::cameraHookTargetAddress()

    );









    /*
     *
     * Camera System Hook
     *
     */


    if(
        !camera::installCameraSystemHook()
    )

    {


        self.getLogger()
        .error(

            "CameraSystem hook failed"

        );



        camera::removeCameraHook();



        mPacketHook.uninstall();



        mPlayerHook.uninstall();



        return false;


    }







    self.getLogger()
    .info(

        "CameraSystem hook installed 0x{:x}",

        camera::cameraSystemAddress()

    );









    /*
     *
     * Register Mod Menu
     *
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


            }


        );








    if(
        !registered
    )

    {


        self.getLogger()
        .error(

            "Failed register ModMenu"

        );



        camera::removeCameraSystemHook();



        camera::removeCameraHook();



        mPacketHook.uninstall();




        mPlayerHook.uninstall();



        return false;


    }








    self.getLogger()
    .info(

        "Levi Native Freecam Ready"

    );





    return true;


}









bool FreecamMod::disable()

{


    restoreAndReset();





    camera::removeCameraSystemHook();





    camera::removeCameraHook();






    mModMenu.unregisterAll();





    mPacketHook.uninstall();





    mPlayerHook.uninstall();








    getSelf()
    .getLogger()
    .info(

        "Levi Freecam disabled"

    );





    return true;


}









bool FreecamMod::unload()

{


    restoreAndReset();






    camera::removeCameraSystemHook();






    camera::removeCameraHook();






    mModMenu.unregisterAll();






    mPacketHook.uninstall();






    mPlayerHook.uninstall();








    getSelf()
    .getLogger()
    .info(

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






    getSelf()
    .getLogger()
    .info(

        "Freecam module {}",

        enabled
        ?
        "ON"
        :
        "OFF"

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





    getSelf()
    .getLogger()
    .info(

        "Camera {}",

        active
        ?
        "ON"
        :
        "OFF"

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
