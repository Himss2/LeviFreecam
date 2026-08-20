#pragma once

#include "ui/ModMenu.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {

class FreecamMod final {
public:
    static FreecamMod &instance();

    FreecamMod();

    [[nodiscard]] ll::mod::NativeMod &getSelf() const noexcept;

    bool load();
    bool enable();
    bool disable();
    bool unload();

private:
    void setFeatureEnabled(bool enabled);

    ll::mod::NativeMod &mSelf;
    ui::ModMenu mModMenu;
};

} // namespace levifreecam
