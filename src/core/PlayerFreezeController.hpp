#pragma once

#include <atomic>


namespace levifreecam {


class PlayerFreezeController final {

public:

    static PlayerFreezeController&
    instance() noexcept;


    /*
     * Aktifkan freeze player
     */
    void enable(
        void* player
    ) noexcept;



    /*
     * Disable freeze
     */
    void disable()
    noexcept;



    /*
     * Dipanggil setiap tick player
     */
    void tick(
        void* player
    ) noexcept;



    [[nodiscard]]
    bool enabled()
    const noexcept;



private:

    PlayerFreezeController() = default;



private:


    /*
     * Status freeze
     */
    std::atomic_bool
        mEnabled{
            false
        };



    /*
     * LocalPlayer pointer
     */
    std::atomic<void*>
        mPlayer{
            nullptr
        };



    /*
     * Backup posisi player

     * Nanti akan diganti hasil RE Actor
     */
    float
        mPosition[3]{
            0.0f,
            0.0f,
            0.0f
        };



    /*
     * Backup rotasi
     */
    float
        mRotation[2]{
            0.0f,
            0.0f
        };

};


}
