#pragma once

#include <cstdint>

namespace levifreecam::memory {

struct CameraTargets final {
    std::uintptr_t activeCameraTransform{0};
};

[[nodiscard]]
bool resolveCameraTargets(
    CameraTargets& targets
) noexcept;

} // namespace levifreecam::memory
