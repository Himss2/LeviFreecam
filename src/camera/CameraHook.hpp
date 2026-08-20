#pragma once

#include <cstdint>

namespace levifreecam::camera {

[[nodiscard]]
bool installCameraHook()
    noexcept;

void removeCameraHook()
    noexcept;

[[nodiscard]]
bool cameraHookInstalled()
    noexcept;

[[nodiscard]]
std::uintptr_t
cameraHookTargetAddress()
    noexcept;

} // namespace levifreecam::camera
