#pragma once

#include <atomic>
#include <cstdint>

#include "core/PlayerFreezeController.hpp"


namespace levifreecam {


class FreecamController final {

public:

    static FreecamController& instance();


    void setModuleEnabled(
        bool enabled
    ) noexcept;


    void setActive(
        bool active
    ) noexcept;


    void onLocalPlayerTick(
        void* localPlayer
    ) noexcept;



    [[nodiscard]]
    bool restoreNow()
    noexcept;



    void forceDisable()
    noexcept;



    void notePlayerAuthInput()
    noexcept;



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



private:


    /*
     * Module state
     */
    std::atomic_bool
        mModuleEnabled{
            false
        };



    /*
     * CAM button
     */
    std::atomic_bool
        mRequestedActive{
            false
        };



    /*
     * Spectator berhasil diterapkan
     */
    std::atomic_bool
        mSpectatorApplied{
            false
        };



    /*
     * Backup gamemode awal
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
     * LocalPlayer pointer
     */
    std::atomic<void*>
        mCurrentPlayer{
            nullptr
        };



    /*
     * refresh spectator
     */
    std::atomic_uint32_t
        mSpectatorRefreshTicks{
            0
        };



    /*
     * Packet counter
     */
    std::atomic_uint64_t
        mPlayerAuthInputSeen{
            0
        };

};


}
