#pragma once

#include <atomic>
#include <cstdint>

#include "core/PlayerFreezeController.hpp"


namespace levifreecam {


class FreecamController final {

public:

    static FreecamController&
    instance();


    /*
     * Enable / disable module
     */
    void setModuleEnabled(
        bool enabled
    ) noexcept;



    /*
     * CAM button state
     */
    void setActive(
        bool active
    ) noexcept;



    /*
     * Called from LocalPlayer tick hook
     */
    void onLocalPlayerTick(
        void* localPlayer
    ) noexcept;



    /*
     * Restore player/camera state
     */
    [[nodiscard]]
    bool restoreNow()
    noexcept;



    /*
     * Force shutdown
     */
    void forceDisable()
    noexcept;



    /*
     * Packet counter
     */
    void notePlayerAuthInput()
    noexcept;



    [[nodiscard]]
    bool moduleEnabled()
    const noexcept;



    [[nodiscard]]
    bool active()
    const noexcept;



    /*
     * Native freecam active state
     */
    [[nodiscard]]
    bool cameraActive()
    const noexcept;



    /*
     * Packet suppression state
     */
    [[nodiscard]]
    bool shouldSuppressPlayerAuthInput()
    const noexcept;



    [[nodiscard]]
    std::uint64_t
    playerAuthInputSeen()
    const noexcept;



private:


    FreecamController() = default;



    void clearSessionState()
    noexcept;



private:


    /*
     * Module enabled
     */
    std::atomic_bool
        mModuleEnabled{
            false
        };



    /*
     * CAM toggle
     */
    std::atomic_bool
        mRequestedActive{
            false
        };



    /*
     * Current LocalPlayer
     */
    std::atomic<void*>
        mCurrentPlayer{
            nullptr
        };



    /*
     * PlayerAuthInput counter
     */
    std::atomic_uint64_t
        mPlayerAuthInputSeen{
            0
        };

};


}
