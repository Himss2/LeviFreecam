#include "camera/CameraHook.hpp"


#include "camera/CameraEntityTracker.hpp"
#include "camera/NativeCameraController.hpp"
#include "camera/CameraState.hpp"

#include "memory/CameraResolver.hpp"


#include <pl/memory/Hook.hpp>


#include <android/log.h>


#include <cstdint>




namespace levifreecam::camera {



namespace {



constexpr char kLogTag[] =
    "Levi Freecam";





using ActiveCameraTransformFn =
    void (*)(
        void* shakeSupport,
        void* cameraComponent,
        void* runtimeContext
    );





ActiveCameraTransformFn
gOriginalActiveCameraTransform =
    nullptr;





void*
gHookTarget =
    nullptr;





std::uintptr_t
gTargetAddress =
    0;





bool
gInstalled =
    false;





std::uint64_t
gCallCounter =
    0;





void*
gLastCameraComponent =
    nullptr;









void activeCameraTransformDetour(

    void* shakeSupport,

    void* cameraComponent,

    void* runtimeContext

)
{


    /*
     *
     * Jalankan kamera vanilla dulu
     *
     * agar:
     *
     * - matrix kamera valid
     * - rotasi valid
     * - interpolasi tetap berjalan
     *
     */


    if(
        gOriginalActiveCameraTransform
        != nullptr
    )
    {

        gOriginalActiveCameraTransform(

            shakeSupport,

            cameraComponent,

            runtimeContext

        );

    }







    if(
        cameraComponent == nullptr
    )
    {
        return;
    }







    gCallCounter++;








    /*
     *
     * Track pergantian CameraComponent
     *
     */


    if(
        cameraComponent
        !=
        gLastCameraComponent
    )
    {


        gLastCameraComponent =
            cameraComponent;




        __android_log_print(

            ANDROID_LOG_INFO,

            kLogTag,

            "Camera component changed : %p",

            cameraComponent

        );


    }








    if(
        (gCallCounter % 300)
        == 0
    )
    {

        __android_log_print(

            ANDROID_LOG_INFO,

            kLogTag,

            "Camera transform active %llu",

            gCallCounter

        );

    }









    auto& state =
        getCameraState();








    /*
     *
     * Freecam OFF
     *
     */


    if(
        !state.enabled.load()
    )
    {

        return;

    }









    /*
     *
     * Simpan camera component
     *
     * dan ambil kontrol transform
     *
     */


    CameraEntityTracker::

        instance()

        .track(

            cameraComponent

        );






    NativeCameraController::

        instance()

        .onCameraTransform(

            cameraComponent

        );









    /*
     *
     * Kamera sudah detached
     *
     */


    if(
        state.detached.load()
    )
    {


        if(
            (gCallCounter % 300)
            == 0
        )
        {


            __android_log_print(

                ANDROID_LOG_INFO,

                kLogTag,

                "FREECAM DETACHED ACTIVE"

            );


        }


    }





}









}





bool installCameraHook()

noexcept

{

    if(
        gInstalled
    )
    {
        return true;
    }








    memory::CameraTargets

    targets{};








    if(
        !memory::resolveCameraTargets(

            targets

        )
    )
    {


        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "Failed resolving camera targets"

        );


        return false;

    }









    if(
        targets.activeCameraTransform
        ==
        0
    )
    {


        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "Camera transform address invalid"

        );


        return false;

    }









    void*

    target =

        reinterpret_cast<void*>(

            targets.activeCameraTransform

        );









    void*

    detour =

        reinterpret_cast<void*>(

            &activeCameraTransformDetour

        );









    void*

    original =

        nullptr;









    const int result =


        pl::memory::hook(

            target,

            detour,

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

            "Camera hook failed %d",

            result

        );


        return false;

    }









    gOriginalActiveCameraTransform =

        reinterpret_cast<ActiveCameraTransformFn>(

            original

        );









    gHookTarget =

        target;









    gTargetAddress =

        targets.activeCameraTransform;









    gInstalled =

        true;









    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "Camera hook installed %p",

        target

    );









    return true;


}









void removeCameraHook()

noexcept

{

    if(
        !gInstalled
    )
    {

        return;

    }









    if(
        gHookTarget != nullptr
    )
    {


        pl::memory::unhook(

            gHookTarget,

            reinterpret_cast<void*>(

                &activeCameraTransformDetour

            )

        );


    }









    gOriginalActiveCameraTransform =
        nullptr;





    gHookTarget =
        nullptr;





    gTargetAddress =
        0;





    gCallCounter =
        0;





    gLastCameraComponent =
        nullptr;





    gInstalled =
        false;









    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "Camera hook removed"

    );


}









bool cameraHookInstalled()

noexcept

{

    return gInstalled;

}









std::uintptr_t cameraHookTargetAddress()

noexcept

{

    return gTargetAddress;

}



}
