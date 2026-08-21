#include "memory/CameraResolver.hpp"


#include <pl/memory/Signature.hpp>


#include <string_view>


namespace levifreecam::memory {


namespace {


constexpr std::string_view kMinecraftLibrary =
    "libminecraftpe.so";



/*
 * Minecraft Bedrock 1.26.44.3 ARM64
 *
 * CameraShakeApplyToCameraSystem helper
 *
 * ABI:
 *
 * x0 = CameraShakeSupport
 * x1 = CameraComponent*
 * x2 = RuntimeContext
 *
 */

constexpr std::string_view
kActiveCameraTransformSignature =

    "FF 03 02 D1 "
    "FD 7B 05 A9 "
    "F6 57 06 A9 "
    "F4 4F 07 A9 "
    "FD 43 01 91 "
    "56 D0 3B D5 "
    "F5 03 00 AA "
    "E0 03 02 AA";


}



bool resolveCameraTargets(
    CameraTargets& targets
) noexcept
{

    targets = {};



    const std::uintptr_t address =

        pl::memory::resolveSignature(
            kActiveCameraTransformSignature,
            kMinecraftLibrary
        );



    if(
        address < 0x10000
    )
    {
        return false;
    }



    targets.activeCameraTransform =
        address;



    return true;

}


}
