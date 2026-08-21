#include "camera/CameraSystemHook.hpp"

#include "camera/NativeCameraController.hpp"

#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>

#include <cstdint>
#include <string_view>


namespace levifreecam::camera {


namespace {


constexpr std::string_view kMinecraftLibrary =
    "libminecraftpe.so";


// Nanti diganti signature final hasil RE
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


UpdateCameraFn gOriginal = nullptr;


void* gTarget = nullptr;


std::uintptr_t gAddress = 0;


bool gInstalled = false;



void updatePlayerFromCameraDetour(
    void* system,
    void* registry
)
{

    if(gOriginal)
    {
        gOriginal(
            system,
            registry
        );
    }


    /*
     * Vanilla camera sudah selesai update.
     *
     * Titik ini tempat Freecam
     * menimpa transform.
     */

    NativeCameraController::
        instance()
        .update();

}



}



bool installCameraSystemHook() noexcept
{

    if(gInstalled)
        return true;


    auto address =
        pl::memory::resolveSignature(
            kUpdatePlayerFromCameraSignature,
            kMinecraftLibrary
        );


    if(address == 0)
        return false;



    void* original = nullptr;


    int result =
        pl::memory::hook(
            reinterpret_cast<void*>(address),
            reinterpret_cast<void*>(
                &updatePlayerFromCameraDetour
            ),
            &original,
            pl::memory::HookPriority::Normal
        );


    if(result != 0 || !original)
        return false;



    gOriginal =
        reinterpret_cast<UpdateCameraFn>(
            original
        );


    gTarget =
        reinterpret_cast<void*>(
            address
        );


    gAddress = address;


    gInstalled = true;


    return true;
}



void removeCameraSystemHook() noexcept
{

    if(!gInstalled)
        return;


    pl::memory::unhook(
        gTarget,
        reinterpret_cast<void*>(
            &updatePlayerFromCameraDetour
        )
    );


    gOriginal = nullptr;
    gTarget = nullptr;
    gAddress = 0;
    gInstalled = false;
}



bool cameraSystemHookInstalled() noexcept
{
    return gInstalled;
}



std::uintptr_t cameraSystemAddress() noexcept
{
    return gAddress;
}


}
