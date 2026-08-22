#pragma once

#include <atomic>
#include <cstdint>


namespace levifreecam {


class PlayerFreezeController final {

public:

    static PlayerFreezeController&
    instance() noexcept;


    void enable(
        void* player
    ) noexcept;


    void disable()
    noexcept;


    void tick(
        void* player
    ) noexcept;


    [[nodiscard]]
    bool enabled()
    const noexcept;



    [[nodiscard]]
    void*
    player()
    const noexcept;



private:

    PlayerFreezeController() = default;



private:


    std::atomic_bool
        mEnabled{
            false
        };


    std::atomic<void*>
        mPlayer{
            nullptr
        };


    /*
     * Session counter
     *
     * Dipakai agar freeze
     * hanya satu kali capture.
     */
    std::atomic_bool
        mCaptured{
            false
        };


    /*
     * Backup state
     *
     * Nanti diisi hasil RE Actor.
     */
    float
        mPosition[3]{

            0.0f,
            0.0f,
            0.0f
        };


    float
        mRotation[2]{

            0.0f,
            0.0f
        };


    float
        mVelocity[3]{

            0.0f,
            0.0f,
            0.0f
        };

};


        }
