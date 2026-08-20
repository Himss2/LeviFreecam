#pragma once

#include <functional>
#include <string>

namespace levifreecam::ui {

class ModMenu final {
public:
    using ToggleCallback = std::function<void(bool)>;

    bool registerAll(std::string modId, ToggleCallback onToggle);
    void unregisterAll() noexcept;

    [[nodiscard]] bool registered() const noexcept { return mRegistered; }

private:
    std::string mModId;
    ToggleCallback mOnToggle;
    bool mRegistered{false};
};

} // namespace levifreecam::ui
