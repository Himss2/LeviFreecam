#include "camera/CameraHook.hpp"

#include "camera/NativeCameraController.hpp"
#include "memory/CameraResolver.hpp"

#include <pl/memory/Hook.hpp>

#include <cstdint>

namespace levifreecam::camera {

namespace {

/*
 * RE prototype:
 *
 * x0 = CameraShakeSupport data
 * x1 = active CameraComponent*
 * x2 = runtime context
 *
 * Kita hanya menggunakan x1.
 */
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


void activeCameraTransformDetour(
    void* shakeSupport,
    void* cameraComponent,
    void* runtimeContext
) {

    const auto original =
        gOriginalActiveCameraTransform;

    if (
        original != nullptr
    ) {

        /*
         * Jalankan kalkulasi kamera Minecraft dahulu.
         */
        original(
            shakeSupport,
            cameraComponent,
            runtimeContext
        );
    }

    /*
     * Kemudian apply Native Freecam.
     */
    NativeCameraController::
        instance().
        onCameraTransform(
            cameraComponent
        );
}

} // namespace


bool installCameraHook()
    noexcept {

    if (gInstalled) {
        return true;
    }

    memory::CameraTargets
        targets{};

    if (
        !memory::resolveCameraTargets(
            targets
        )
    ) {
        return false;
    }

    if (
        targets.activeCameraTransform ==
        0
    ) {
        return false;
    }

    void* original =
        nullptr;

    void* target =
        reinterpret_cast<void*>(
            targets.activeCameraTransform
        );

    void* detour =
        reinterpret_cast<void*>(
            &activeCameraTransformDetour
        );

    const int result =

        pl::memory::hook(
            target,
            detour,
            &original,

            pl::memory::
                HookPriority::Normal
        );

    if (
        result != 0 ||
        original == nullptr
    ) {
        return false;
    }

    gOriginalActiveCameraTransform =

        reinterpret_cast<
            ActiveCameraTransformFn
        >(
            original
        );

    gHookTarget =
        target;

    gTargetAddress =
        targets.activeCameraTransform;

    gInstalled =
        true;

    return true;
}


void removeCameraHook()
    noexcept {

    if (
        !gInstalled
    ) {
        return;
    }

    if (
        gHookTarget != nullptr
    ) {

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

    gInstalled =
        false;
}


bool cameraHookInstalled()
    noexcept {

    return gInstalled;
}


std::uintptr_t
cameraHookTargetAddress()
    noexcept {

    return gTargetAddress;
}

} // namespace levifreecam::camera
