#pragma once

#include <cstdint>

namespace levifreecam::camera {

bool installCameraSystemHook() noexcept;

void removeCameraSystemHook() noexcept;

bool cameraSystemHookInstalled() noexcept;

std::uintptr_t cameraSystemAddress() noexcept;

}
