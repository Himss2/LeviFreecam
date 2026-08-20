#pragma once

#include <functional>
#include <string>

namespace levifreecam::ui {

class ModMenu final {
public:
    using ModuleToggleCallback =
        std::function<void(bool)>;

    using CameraToggleCallback =
        std::function<void(bool)>;

    bool registerAll(
        std::string modId,
        ModuleToggleCallback onModuleToggle,
        CameraToggleCallback onCameraToggle
    );

    void unregisterAll() noexcept;

    [[nodiscard]]
    bool registered() const noexcept {
        return mRegistered;
    }

private:
    std::string mModId;

    ModuleToggleCallback
        mOnModuleToggle;

    CameraToggleCallback
        mOnCameraToggle;

    bool mRegistered{false};
};

} // namespace levifreecam::ui
