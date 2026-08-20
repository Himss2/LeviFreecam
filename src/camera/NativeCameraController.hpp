#pragma once

#include "camera/CameraState.hpp"

#include <atomic>

namespace levifreecam::camera {

class NativeCameraController final {

public:

    static NativeCameraController&
    instance() noexcept;

    bool enable() noexcept;

    bool disable() noexcept;

    [[nodiscard]]
    bool isEnabled()
        const noexcept;

    [[nodiscard]]
    bool cameraCaptured()
        const noexcept;

    /*
     * Dipanggil dari LocalPlayer::normalTick.
     *
     * Nanti menjadi integration point
     * untuk movement.
     */
    void update() noexcept;

    /*
     * Dipanggil CameraHook setelah
     * Minecraft selesai menghitung
     * transform kamera.
     */
    void onCameraTransform(
        void* cameraComponent
    ) noexcept;

    void translate(
        float x,
        float y,
        float z
    ) noexcept;

    void requestRecapture()
        noexcept;

private:

    NativeCameraController() =
        default;

    std::atomic<void*>
        mLastCameraComponent{
            nullptr
        };
};

} // namespace levifreecam::camera
