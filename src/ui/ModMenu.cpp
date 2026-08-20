#include "ui/ModMenu.hpp"

#include <pl/ModMenu.hpp>

#include <string_view>
#include <utility>

namespace levifreecam::ui {

namespace {

constexpr std::string_view kModuleId =
    "levi_freecam.freecam";

constexpr std::string_view kButtonId =
    "levi_freecam.freecam.cam";

} // namespace


bool ModMenu::registerAll(
    std::string modId,
    ModuleToggleCallback onModuleToggle,
    CameraToggleCallback onCameraToggle
) {

    if (mRegistered) {
        return true;
    }

    mModId =
        std::move(modId);

    mOnModuleToggle =
        std::move(onModuleToggle);

    mOnCameraToggle =
        std::move(onCameraToggle);

    /*
     * =====================================================
     * FREECAM MODULE
     * =====================================================
     */
    const bool moduleRegistered =

        pl::modmenu::ModuleBuilder(
            std::string(kModuleId),
            "Freecam"
        )

        .modId(
            mModId
        )

        .description(
            "Free camera for Levi Launcher. "
            "Enable this module to display the "
            "CAM button."
        )

        .defaultEnabled(
            false
        )

        .onToggle(
            [this](
                std::string_view moduleId,
                bool enabled
            ) {

                if (
                    moduleId !=
                    kModuleId
                ) {
                    return;
                }

                if (
                    mOnModuleToggle
                ) {
                    mOnModuleToggle(
                        enabled
                    );
                }
            }
        )

        .registerModule();


    if (!moduleRegistered) {

        mOnModuleToggle = {};
        mOnCameraToggle = {};

        mModId.clear();

        return false;
    }


    /*
     * =====================================================
     * CAM BUTTON
     * =====================================================
     *
     * We deliberately use Levi's Keycap preset.
     *
     * It gives us the same Minecraft-style:
     *
     * gray button
     * dark outline
     * square corners
     * bold label
     * lighter pressed/active state
     *
     * No custom ImGui.
     * No custom Android View.
     * No custom SVG required.
     */
    const bool buttonRegistered =

        pl::modmenu::ButtonBuilder(
            std::string(kButtonId),
            "Freecam Camera"
        )

        .moduleId(
            std::string(kModuleId)
        )

        .modId(
            mModId
        )

        /*
         * Visible text.
         */
        .label(
            "CAM"
        )

        /*
         * Press once:
         *
         * CAM ON
         *
         * Press again:
         *
         * CAM OFF
         */
        .behavior(
            pl::modmenu::
                ButtonBehavior::Toggle
        )

        /*
         * Native Levi / Minecraft-looking
         * keycap style.
         */
        .stylePreset(
            pl::modmenu::
                ButtonStylePreset::Keycap
        )

        /*
         * CAM is 3 letters.
         *
         * Levi itself uses approximately
         * 1.8 width for labels <= 4 chars.
         */
        .sizeScale(
            1.8f,
            1.0f
        )

        .onEvent(
            [this](
                std::string_view buttonId,
                pl::modmenu::ButtonEvent event,
                float value
            ) {

                if (
                    buttonId !=
                    kButtonId
                ) {
                    return;
                }

                if (
                    event !=
                    pl::modmenu::
                        ButtonEvent::StateChanged
                ) {
                    return;
                }

                const bool active =
                    value > 0.5f;

                if (
                    mOnCameraToggle
                ) {
                    mOnCameraToggle(
                        active
                    );
                }
            }
        )

        .registerButton();


    if (!buttonRegistered) {

        pl::modmenu::
            unregisterModule(
                kModuleId
            );

        mOnModuleToggle = {};
        mOnCameraToggle = {};

        mModId.clear();

        return false;
    }


    mRegistered = true;

    return true;
}


void ModMenu::unregisterAll()
    noexcept {

    if (!mRegistered) {
        return;
    }

    /*
     * Always remove button first.
     */
    pl::modmenu::
        unregisterButton(
            kButtonId
        );

    pl::modmenu::
        unregisterModule(
            kModuleId
        );

    mRegistered = false;

    mOnModuleToggle = {};
    mOnCameraToggle = {};

    mModId.clear();
}

} // namespace levifreecam::ui
