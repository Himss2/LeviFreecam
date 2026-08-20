#pragma once

#include <atomic>

namespace levifreecam {

// v0.1.0 only owns the feature state. Minecraft interaction is deliberately
// absent until the hook and local-spectator layers are implemented.
class FreecamController final {
public:
    static FreecamController &instance();

    void setEnabled(bool enabled) noexcept;
    void forceDisable() noexcept;

    [[nodiscard]] bool enabled() const noexcept;

private:
    FreecamController() = default;

    std::atomic_bool mEnabled{false};
};

} // namespace levifreecam
