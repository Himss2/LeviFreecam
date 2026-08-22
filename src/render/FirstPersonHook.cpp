#include "render/FirstPersonHook.hpp"


#include <android/log.h>



namespace levifreecam::render {



namespace {


constexpr char kLogTag[] =
    "Levi Freecam";



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

    if(
        mInstalled.load()
    )
    {
        return true;
    }



    /*
     * Placeholder tahap pertama.
     *
     * Target render first person
     * belum dipasang karena signature
     * render arm/item belum ditemukan.
     *
     * File ini hanya menyiapkan
     * lifecycle hook.
     */



    mInstalled.store(
        true
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "FirstPersonHook initialized"
    );



    return true;

}









void FirstPersonHook::uninstall()
noexcept
{

    if(
        !mInstalled.load()
    )
    {
        return;
    }



    mEnabled.store(
        false
    );



    mInstalled.store(
        false
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "FirstPersonHook removed"
    );


}









void FirstPersonHook::setEnabled(
    bool enabled
)
noexcept
{

    mEnabled.store(
        enabled
    );



    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "FirstPerson render {}",
        enabled
        ? "disabled"
        : "enabled"
    );

}









bool FirstPersonHook::enabled()
const noexcept
{

    return mEnabled.load();

}





}
