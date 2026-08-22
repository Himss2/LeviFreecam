#include "render/FirstPersonHook.hpp"


#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>


#include <android/log.h>


#include <string_view>
#include <cstdint>



namespace levifreecam::render {


namespace {


constexpr char kLogTag[] =
    "Levi Freecam";



constexpr std::string_view
kMinecraftLibrary =
    "libminecraftpe.so";



/*
 * Placeholder signature.
 *
 * Harus diganti setelah RE fungsi:
 *
 * ClientInstance::getCameraEntity
 *
 * atau
 *
 * FirstPersonRenderer::render
 *
 */


constexpr std::string_view
kFirstPersonRenderSignature =

    "? ? ? A9 "
    "? ? ? A9 "
    "? ? ? A9 "
    "FD 03 00 91";





using FirstPersonRenderFn =
    void(*)(
        void* renderer,
        void* context
    );



FirstPersonRenderFn
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



bool
gEnabled =
    false;





void firstPersonRenderDetour(
    void* renderer,
    void* context
)
{


    /*
     * Freecam aktif:
     *
     * jangan render tangan
     * jangan render item first person
     */


    if(gEnabled)
    {

        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "FirstPerson render blocked"
        );


        return;

    }




    if(gOriginal)
    {

        gOriginal(
            renderer,
            context
        );

    }


}



}



FirstPersonHook&
FirstPersonHook::instance()
noexcept
{

    static FirstPersonHook hook;

    return hook;

}





bool FirstPersonHook::install()
noexcept
{


    if(gInstalled)
    {
        return true;
    }



    auto address =
        pl::memory::resolveSignature(
            kFirstPersonRenderSignature,
            kMinecraftLibrary
        );



    if(address == 0)
    {


        __android_log_print(
            ANDROID_LOG_ERROR,
            kLogTag,
            "FirstPerson signature not found"
        );


        return false;

    }





    void* original =
        nullptr;




    int result =
        pl::memory::hook(

            reinterpret_cast<void*>(
                address
            ),

            reinterpret_cast<void*>(
                &firstPersonRenderDetour
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
            "FirstPerson hook failed %d",
            result
        );


        return false;

    }





    gOriginal =
        reinterpret_cast<
            FirstPersonRenderFn
        >(
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
        "FirstPerson hook installed 0x%lx",
        gAddress
    );



    return true;

}







void FirstPersonHook::uninstall()
noexcept
{


    if(!gInstalled)
    {
        return;
    }




    pl::memory::unhook(

        gTarget,

        reinterpret_cast<void*>(
            &firstPersonRenderDetour
        )

    );



    gOriginal =
        nullptr;



    gTarget =
        nullptr;



    gAddress =
        0;



    gInstalled =
        false;



}





void FirstPersonHook::setEnabled(
    bool enabled
)
noexcept
{


    gEnabled =
        enabled;



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "FirstPerson render %s",
        enabled
        ?
        "BLOCK"
        :
        "NORMAL"
    );


}






bool FirstPersonHook::isEnabled()
const noexcept
{

    return gEnabled;

}





bool FirstPersonHook::installed()
const noexcept
{

    return gInstalled;

}





std::uintptr_t
FirstPersonHook::targetAddress()
noexcept
{

    return gAddress;

}




}
