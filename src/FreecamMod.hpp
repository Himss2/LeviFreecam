#pragma once

#include "hooks/PacketHook.hpp"
#include "ui/ModMenu.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {

class FreecamMod final {
public:
    static FreecamMod& instance();

    FreecamMod();

    [[nodiscard]]
    ll::mod::NativeMod& getSelf() const noexcept;

    bool load();
    bool enable();
    bool disable();
    bool unload();

private:
    void setModuleEnabled(bool enabled);
    void setCameraActive(bool active);

    ll::mod::NativeMod& mSelf;

    hooks::PacketHook mPacketHook;
    ui::ModMenu mModMenu;
};

} // namespace levifreecam
