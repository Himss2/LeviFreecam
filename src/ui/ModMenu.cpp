#include "ui/ModMenu.hpp"

#include "core/FreecamController.hpp"

#include <string_view>
#include <utility>

#include <pl/ModMenu.hpp>

namespace levifreecam::ui {
namespace {

constexpr std::string_view kModuleId = "levi_freecam.freecam";
constexpr std::string_view kButtonId = "levi_freecam.freecam_button";

} // namespace

bool ModMenu::registerAll(std::string modId, ToggleCallback onToggle) {
    if (mRegistered) {
        return true;
    }

    mModId = std::move(modId);
    mOnToggle = std::move(onToggle);

    const bool moduleRegistered =
        pl::modmenu::ModuleBuilder(std::string(kModuleId), "Freecam")
            .modId(mModId)
            .description(
                "Foundation build. The toggle is wired to Levi's native Mod Menu; "
                "Minecraft camera/network hooks are added in later versions.")
            .defaultEnabled(false)
            .onToggle([this](std::string_view moduleId, bool enabled) {
                if (moduleId != kModuleId) {
                    return;
                }
                if (mOnToggle) {
                    mOnToggle(enabled);
                }
            })
            .registerModule();

    if (!moduleRegistered) {
        mOnToggle = {};
        mModId.clear();
        return false;
    }

    // Click behavior is intentional in v0.1.0: the button delegates state to
    // the module itself via setModuleEnabled(), giving us one source of truth.
    const bool buttonRegistered =
        pl::modmenu::ButtonBuilder(std::string(kButtonId), "Freecam")
            .moduleId(std::string(kModuleId))
            .modId(mModId)
            .label("FC")
            .behavior(pl::modmenu::ButtonBehavior::Click)
            .stylePreset(pl::modmenu::ButtonStylePreset::Accent)
            .sizeScale(1.0f, 1.0f)
            .onEvent([](std::string_view buttonId,
                        pl::modmenu::ButtonEvent event,
                        float /*value*/) {
                if (buttonId != kButtonId ||
                    event != pl::modmenu::ButtonEvent::Click) {
                    return;
                }

                auto &controller = FreecamController::instance();
                pl::modmenu::setModuleEnabled(
                    kModuleId, !controller.enabled());
            })
            .registerButton();

    if (!buttonRegistered) {
        pl::modmenu::unregisterModule(kModuleId);
        mOnToggle = {};
        mModId.clear();
        return false;
    }

    mRegistered = true;
    return true;
}

void ModMenu::unregisterAll() noexcept {
    if (!mRegistered) {
        return;
    }

    pl::modmenu::unregisterButton(kButtonId);
    pl::modmenu::unregisterModule(kModuleId);

    mRegistered = false;
    mOnToggle = {};
    mModId.clear();
}

} // namespace levifreecam::ui
