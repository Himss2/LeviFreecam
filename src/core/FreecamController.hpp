#pragma once

#include <atomic>
#include <cstdint>

namespace levifreecam {

class FreecamController final {
public:
    static FreecamController& instance();

    /*
     * Toggle utama dari Mod Menu.
     */
    void setModuleEnabled(
        bool enabled
    ) noexcept;

    /*
     * Toggle CAM.
     */
    void setActive(
        bool active
    ) noexcept;

    /*
     * Dipanggil dari LocalPlayer normalTick.
     */
    void onLocalPlayerTick(
        void* localPlayer
    ) noexcept;

    /*
     * Restore darurat saat native mod
     * di-disable/unload.
     */
    [[nodiscard]]
    bool restoreNow() noexcept;

    /*
     * Reset seluruh state.
     *
     * Panggil sesudah restoreNow()
     * jika spectator pernah diterapkan.
     */
    void forceDisable() noexcept;

    /*
     * Debug counter.
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
     * Master Mod Menu state.
     */
    std::atomic_bool
        mModuleEnabled{
            false
        };

    /*
     * CAM requested state.
     */
    std::atomic_bool
        mRequestedActive{
            false
        };

    /*
     * Menandakan client benar-benar sudah
     * masuk local spectator.
     *
     * PacketHook hanya boleh memblokir
     * PlayerAuthInput jika ini true.
     */
    std::atomic_bool
        mSpectatorApplied{
            false
        };

    /*
     * Gamemode player sebelum Freecam.
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
     * LocalPlayer pointer terakhir.
     */
    std::atomic<void*>
        mCurrentPlayer{
            nullptr
        };

    /*
     * Ambient mengulang spectator state
     * secara periodik.
     *
     * Kita melakukan hal serupa.
     */
    std::atomic_uint32_t
        mSpectatorRefreshTicks{
            0
        };

    std::atomic_uint64_t
        mPlayerAuthInputSeen{
            0
        };
};

} // namespace levifreecam
