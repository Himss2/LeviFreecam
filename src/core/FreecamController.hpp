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


    std::atomic_bool
        mModuleEnabled{
            false
        };


    std::atomic_bool
        mRequestedActive{
            false
        };



    std::atomic<void*>
        mCurrentPlayer{
            nullptr
        };



    std::atomic_uint64_t
        mPlayerAuthInputSeen{
            0
        };


};


}
