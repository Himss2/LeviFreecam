#include "camera/NativeCameraController.hpp"

#include "camera/CameraController.hpp"

namespace levifreecam::camera {

namespace {

CameraState gCameraState;

} // namespace


CameraState&
getCameraState() noexcept {

    return gCameraState;
}


NativeCameraController&
NativeCameraController::instance()
    noexcept {

    static NativeCameraController
        controller;

    return controller;
}


bool NativeCameraController::enable()
    noexcept {

    auto& state =
        getCameraState();

    if (
        state.enabled.load(
            std::memory_order_acquire
        )
    ) {
        return true;
    }

    /*
     * Kamera asli akan dicapture
     * pada callback CameraHook berikutnya.
     */
    state.captured.store(
        false,
        std::memory_order_release
    );

    state.captureRequested.store(
        true,
        std::memory_order_release
    );

    state.enabled.store(
        true,
        std::memory_order_release
    );

    return true;
}


bool NativeCameraController::disable()
    noexcept {

    auto& state =
        getCameraState();

    state.enabled.store(
        false,
        std::memory_order_release
    );

    state.captureRequested.store(
        false,
        std::memory_order_release
    );

    state.captured.store(
        false,
        std::memory_order_release
    );

    mLastCameraComponent.store(
        nullptr,
        std::memory_order_release
    );

    /*
     * Tidak perlu menulis posisi vanilla kembali.
     *
     * Tick berikutnya Minecraft otomatis
     * menguasai CameraComponent lagi.
     */
    return true;
}


bool NativeCameraController::isEnabled()
    const noexcept {

    return
        getCameraState().
        enabled.load(
            std::memory_order_acquire
        );
}


bool
NativeCameraController::cameraCaptured()
    const noexcept {

    return
        getCameraState().
        captured.load(
            std::memory_order_acquire
        );
}


void NativeCameraController::update()
    noexcept {

    /*
     * Belum melakukan movement.
     *
     * Posisi diterapkan melalui
     * onCameraTransform().
     *
     * Fungsi ini sengaja dipertahankan
     * sebagai game-thread integration point
     * untuk CameraFlyMove berikutnya.
     */
}


void
NativeCameraController::onCameraTransform(
    void* cameraComponent
) noexcept {

    if (
        cameraComponent == nullptr
    ) {
        return;
    }

    auto& state =
        getCameraState();

    if (
        !state.enabled.load(
            std::memory_order_acquire
        )
    ) {
        return;
    }

    mLastCameraComponent.store(
        cameraComponent,
        std::memory_order_release
    );

    auto& camera =
        CameraController::instance();

    const bool needsCapture =

        state.captureRequested.exchange(
            false,
            std::memory_order_acq_rel
        )

        ||

        !state.captured.load(
            std::memory_order_acquire
        );

    if (needsCapture) {

        Vec3 position{};

        CameraOrientation
            orientation{};

        if (
            !camera.readPosition(
                cameraComponent,
                position
            )
        ) {

            state.captureRequested.store(
                true,
                std::memory_order_release
            );

            return;
        }

        if (
            !camera.readOrientation(
                cameraComponent,
                orientation
            )
        ) {

            state.captureRequested.store(
                true,
                std::memory_order_release
            );

            return;
        }

        state.position =
            position;

        state.orientation =
            orientation;

        state.captured.store(
            true,
            std::memory_order_release
        );

        /*
         * Frame pertama menggunakan
         * transform vanilla persis.
         */
        return;
    }

    /*
     * ======================================================
     * NATIVE FREECAM V1
     * ======================================================
     *
     * Minecraft sudah menghitung:
     *
     * - attach
     * - camera shake
     * - vanilla player position
     *
     * tetapi setelah itu posisi kamera
     * kita kembalikan ke state Freecam.
     *
     * Player tidak disentuh.
     */
    camera.writePosition(
        cameraComponent,
        state.position
    );
}


void NativeCameraController::translate(
    float x,
    float y,
    float z
) noexcept {

    auto& state =
        getCameraState();

    if (
        !state.enabled.load(
            std::memory_order_acquire
        )

        ||

        !state.captured.load(
            std::memory_order_acquire
        )
    ) {
        return;
    }

    state.position.x += x;
    state.position.y += y;
    state.position.z += z;
}


void NativeCameraController::requestRecapture()
    noexcept {

    auto& state =
        getCameraState();

    state.captured.store(
        false,
        std::memory_order_release
    );

    state.captureRequested.store(
        true,
        std::memory_order_release
    );
}

} // namespace levifreecam::camera
