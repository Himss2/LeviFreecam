#include "camera/CameraSystemHook.hpp"


#include "camera/NativeCameraController.hpp"


#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>


#include <android/log.h>


#include <cstdint>
#include <string_view>




namespace levifreecam::camera {



namespace {



constexpr char kLogTag[] =
    "Levi Freecam";



constexpr std::string_view kMinecraftLibrary =
    "libminecraftpe.so";





/*
 *
 * Prototype signature.
 *
 * Tetap menggunakan milik repo lama.
 *
 */

constexpr std::string_view
kUpdatePlayerFromCameraSignature =

    "? ? ? A9 "
    "? ? ? A9 "
    "? ? ? A9 "
    "FD 03 00 91 "
    "? ? ? AA";





using UpdateCameraFn =
    void (*)(
        void* system,
        void* registry
    );





UpdateCameraFn
gOriginal =
    nullptr;





void*
gTarget =
    nullptr;





std::uintptr_t
gAddress =
    0;





bool
gInstalled =
    false;





void*
gLastSystem =
    nullptr;





void*
gLastRegistry =
    nullptr;






void updatePlayerFromCameraDetour(

    void* system,

    void* registry

)

{




    /*
     *
     * Simpan data camera system
     *
     */

    if(
        system != gLastSystem
    )
    {


        gLastSystem =
            system;



        __android_log_print(

            ANDROID_LOG_INFO,

            kLogTag,

            "CameraSystem changed : %p",

            system

        );


    }






    if(
        registry != gLastRegistry
    )
    {


        gLastRegistry =
            registry;



        __android_log_print(

            ANDROID_LOG_INFO,

            kLogTag,

            "CameraRegistry changed : %p",

            registry

        );


    }








    /*
     *
     * Jalankan vanilla
     *
     */

    if(
        gOriginal
    )

    {

        gOriginal(

            system,

            registry

        );

    }








    /*
     *
     * Update Freecam
     *
     */

    NativeCameraController::

        instance()

        .update();



}






}








bool installCameraSystemHook()
noexcept

{

    if(
        gInstalled
    )

    {

        return true;

    }






    auto address =

        pl::memory::resolveSignature(

            kUpdatePlayerFromCameraSignature,

            kMinecraftLibrary

        );






    if(
        address == 0
    )

    {

        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "CameraSystem signature failed"

        );


        return false;

    }








    void*
    original =
        nullptr;








    const int result =


        pl::memory::hook(

            reinterpret_cast<void*>(

                address

            ),


            reinterpret_cast<void*>(

                &updatePlayerFromCameraDetour

            ),


            &original,


            pl::memory::HookPriority::Normal

        );









    if(
        result != 0
        ||
        original == nullptr
    )

    {


        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "CameraSystem hook failed %d",

            result

        );


        return false;


    }









    gOriginal =

        reinterpret_cast<UpdateCameraFn>(

            original

        );






    gTarget =

        reinterpret_cast<void*>(

            address

        );






    gAddress =
        address;






    gInstalled =
        true;







    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "CameraSystem hook installed %p",

        gTarget

    );





    return true;


}









void removeCameraSystemHook()
noexcept

{

    if(
        !gInstalled
    )

    {

        return;

    }






    pl::memory::unhook(

        gTarget,


        reinterpret_cast<void*>(

            &updatePlayerFromCameraDetour

        )

    );






    gOriginal =
        nullptr;



    gTarget =
        nullptr;



    gAddress =
        0;



    gLastSystem =
        nullptr;



    gLastRegistry =
        nullptr;



    gInstalled =
        false;



}








bool cameraSystemHookInstalled()
noexcept

{

    return gInstalled;

}







std::uintptr_t cameraSystemAddress()
noexcept

{

    return gAddress;

}







std::uintptr_t lastCameraSystem()
noexcept

{

    return reinterpret_cast<std::uintptr_t>(

        gLastSystem

    );

}







std::uintptr_t lastCameraRegistry()
noexcept

{

    return reinterpret_cast<std::uintptr_t>(

        gLastRegistry

    );

}





}
