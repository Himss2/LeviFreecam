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
 * Build ID:
 * b480c79a54f33d6e4f0d63a131673e3daf749911
 *
 * RE target RVA:
 *
 * 0x1098055C
 *
 * Target merupakan helper dari:
 *
 * CameraShakeApplyToCameraSystem
 *
 * ABI:
 *
 * x0 = camera shake/support data
 * x1 = active MinecraftCamera::CameraComponent*
 * x2 = runtime camera/shake context
 *
 * CameraComponent:
 *
 * +0x28 = orientation, 16 bytes
 * +0x38 = position.x
 * +0x3C = position.y
 * +0x40 = position.z
 *
 * Stride CameraComponent:
 *
 * 0x118
 *
 * Signature ini match tepat 1 kali pada
 * libminecraftpe.so target.
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

} // namespace

bool resolveCameraTargets(
    CameraTargets& targets
) noexcept {

    targets = {};

    const std::uintptr_t
        activeCameraTransform =

            pl::memory::resolveSignature(
                kActiveCameraTransformSignature,
                kMinecraftLibrary
            );

    if (
        activeCameraTransform == 0
    ) {
        return false;
    }

    targets.activeCameraTransform =
        activeCameraTransform;

    return true;
}

} // namespace levifreecam::memory
