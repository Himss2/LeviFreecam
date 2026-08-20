#pragma once

#include <cstdint>

namespace levifreecam::hooks {

class PacketHook final {
public:
    /*
     * Resolve and install:
     *
     * LoopbackPacketSender::sendToServer
     */
    bool install();

    /*
     * Remove hook safely.
     */
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
