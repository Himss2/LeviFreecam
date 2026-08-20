#pragma once

#include <atomic>

namespace levifreecam::camera {

struct Vec3 final {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
};

struct CameraOrientation final {

    float value[4]{
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };
};

struct CameraState final {

    std::atomic_bool
        enabled{false};

    std::atomic_bool
        captureRequested{false};

    std::atomic_bool
        captured{false};

    Vec3 position{};

    CameraOrientation
        orientation{};
};

CameraState&
getCameraState() noexcept;

} // namespace levifreecam::camera
