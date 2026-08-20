#pragma once

#include "hooks/PacketHook.hpp"
#include "hooks/PlayerHook.hpp"
#include "ui/ModMenu.hpp"

#include <pl/Mod.hpp>

namespace levifreecam {

class FreecamMod final {
public:
    static FreecamMod& instance();

    FreecamMod();

    [[nodiscard]]
    ll::mod::NativeMod&
    getSelf() const noexcept;

    bool load();

    bool enable();

    bool disable();

    bool unload();

private:
    void setModuleEnabled(
        bool enabled
    );

    void setCameraActive(
        bool active
    );

    void restoreAndReset();

    ll::mod::NativeMod&
        mSelf;

    hooks::PacketHook
        mPacketHook;

    hooks::PlayerHook
        mPlayerHook;

    ui::ModMenu
        mModMenu;
};

} // namespace levifreecam
