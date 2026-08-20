#pragma once

#include <atomic>
#include <cstdint>

namespace levifreecam {

class FreecamController final {
public:
    static FreecamController& instance();

    /*
     * Master module state.
     *
     * This corresponds to the Freecam toggle
     * inside Levi Mod Menu.
     */
    void setModuleEnabled(
        bool enabled
    ) noexcept;

    /*
     * Actual Freecam activation state.
     *
     * Controlled by the CAM floating button.
     */
    void setActive(
        bool active
    ) noexcept;

    /*
     * Force Freecam OFF.
     *
     * Used during:
     *
     * - module disable
     * - mod unload
     * - world exit later
     */
    void forceDisable() noexcept;

    /*
     * This will be controlled by GameModeController
     * in the next implementation stage.
     *
     * false:
     * do NOT block PlayerAuthInput.
     *
     * true:
     * PlayerAuthInput may safely be blocked.
     */
    void setSpectatorApplied(
        bool applied
    ) noexcept;

    /*
     * Debug counter proving that the packet hook
     * sees PlayerAuthInput.
     */
    void notePlayerAuthInput() noexcept;

    [[nodiscard]]
    bool moduleEnabled() const noexcept;

    [[nodiscard]]
    bool active() const noexcept;

    [[nodiscard]]
    bool spectatorApplied() const noexcept;

    [[nodiscard]]
    bool shouldSuppressPlayerAuthInput()
        const noexcept;

    [[nodiscard]]
    std::uint64_t playerAuthInputSeen()
        const noexcept;

private:
    FreecamController() = default;

    std::atomic_bool
        mModuleEnabled{false};

    std::atomic_bool
        mActive{false};

    std::atomic_bool
        mSpectatorApplied{false};

    std::atomic_uint64_t
        mPlayerAuthInputSeen{0};
};

} // namespace levifreecam
