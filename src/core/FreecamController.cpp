#include "core/FreecamController.hpp"

namespace levifreecam {

FreecamController &FreecamController::instance() {
    static FreecamController controller;
    return controller;
}

void FreecamController::setEnabled(bool enabled) noexcept {
    mEnabled.store(enabled, std::memory_order_release);
}

void FreecamController::forceDisable() noexcept {
    setEnabled(false);
}

bool FreecamController::enabled() const noexcept {
    return mEnabled.load(std::memory_order_acquire);
}

} // namespace levifreecam
