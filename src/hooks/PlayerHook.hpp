#pragma once

#include <cstdint>

namespace levifreecam::hooks {

class PlayerHook final {
public:
    bool install();

    void uninstall() noexcept;

    [[nodiscard]]
    bool installed() const noexcept;

    [[nodiscard]]
    std::uintptr_t targetAddress()
        const noexcept;

private:
    std::uintptr_t
        mTargetAddress{0};

    bool
        mInstalled{false};
};

} // namespace levifreecam::hooks
