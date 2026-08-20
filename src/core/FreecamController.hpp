#pragma once

#include <atomic>
#include <cstdint>

namespace levifreecam {

class FreecamController final {
public:
    static FreecamController& instance();

    /*
     * Master Freecam module state.
     */
    void setModuleEnabled(
        bool enabled
    ) noexcept;

    /*
     * CAM floating button state.
     */
    void setActive(
        bool active
    ) noexcept;

    /*
     * Called from LocalPlayer::normalTick.
     *
     * All direct Minecraft player operations
     * are performed here instead of the UI thread.
     */
    void onLocalPlayerTick(
        void* localPlayer
    ) noexcept;

    /*
     * Restore the original GameType immediately
     * before the native mod is disabled/unloaded.
     */
    [[nodiscard]]
    bool restoreNow() noexcept;

    /*
     * Clear Freecam state.
     */
    void forceDisable() noexcept;

    /*
     * Called by PacketHook whenever a
     * PlayerAuthInput packet is observed.
     */
    void notePlayerAuthInput() noexcept;

    [[nodiscard]]
    bool moduleEnabled()
        const noexcept;

    [[nodiscard]]
    bool active()
        const noexcept;

    [[nodiscard]]
    bool spectatorApplied()
        const noexcept;

    [[nodiscard]]
    bool shouldSuppressPlayerAuthInput()
        const noexcept;

    [[nodiscard]]
    std::uint64_t playerAuthInputSeen()
        const noexcept;

private:
    FreecamController() = default;

    void clearSessionState()
        noexcept;

    /*
     * Freecam module enabled in Mod Menu.
     */
    std::atomic_bool
        mModuleEnabled{
            false
        };

    /*
     * CAM button requested ON.
     */
    std::atomic_bool
        mRequestedActive{
            false
        };

    /*
     * True only after local spectator
     * has actually been applied.
     *
     * PacketHook uses this value to decide
     * when PlayerAuthInput should be dropped.
     */
    std::atomic_bool
        mSpectatorApplied{
            false
        };

    /*
     * Original gamemode backup.
     */
    std::atomic_bool
        mOriginalGameTypeValid{
            false
        };

    std::atomic_int
        mOriginalGameType{
            0
        };

    /*
     * Current LocalPlayer object.
     */
    std::atomic<void*>
        mCurrentPlayer{
            nullptr
        };

    /*
     * Used for the periodic spectator refresh.
     */
    std::atomic_uint32_t
        mSpectatorRefreshTicks{
            0
        };

    /*
     * Number of PlayerAuthInput packets
     * successfully identified by PacketHook.
     *
     * Freecam will never activate until
     * this becomes greater than zero.
     */
    std::atomic_uint64_t
        mPlayerAuthInputSeen{
            0
        };

    /*
     * Prevent repeated warning logs while
     * waiting for PacketHook validation.
     */
    std::atomic_bool
        mWaitingForPacketHookLogged{
            false
        };
};

} // namespace levifreecam
