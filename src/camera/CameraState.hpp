#pragma once

#include <atomic>


namespace levifreecam::camera {


struct Vec3 final {

    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

};



struct CameraOrientation final {

    float value[4]{
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };

};





struct CameraState final {


    /*
     * Native camera state.
     */
    std::atomic_bool
        enabled{
            false
        };



    /*
     * Request capture ulang.
     */
    std::atomic_bool
        captureRequested{
            false
        };



    /*
     * Camera berhasil capture.
     */
    std::atomic_bool
        captured{
            false
        };




    /*
     * Camera detached dari LocalPlayer.
     *
     * Saat true:
     *
     * - camera tidak mengikuti player asli
     * - transform camera dikontrol freecam
     * - vanilla camera update ditahan
     */
    std::atomic_bool
        detached{
            false
        };




    /*
     * Posisi virtual camera.
     *
     * Posisi yang dikontrol oleh Freecam.
     */
    Vec3 position{};




    /*
     * Backup posisi kamera asli Minecraft.
     *
     * Digunakan untuk restore
     * ketika Freecam dimatikan.
     */
    Vec3 originalPosition{};





    /*
     * Kecepatan virtual camera.
     *
     * Dipakai nanti untuk:
     *
     * joystick
     * keyboard
     * touch drag
     */
    Vec3 velocity{};





    /*
     * Rotasi virtual camera.
     */
    CameraOrientation
        orientation{};





    /*
     * Backup rotasi kamera asli Minecraft.
     *
     * Digunakan untuk restore
     * ketika Freecam dimatikan.
     */
    CameraOrientation
        originalOrientation{};





    /*
     * Freecam rotation state.
     *
     * yaw:
     * rotasi horizontal kiri-kanan
     *
     * pitch:
     * rotasi vertikal atas-bawah
     */
    float yaw{
        0.0f
    };



    float pitch{
        0.0f
    };





    /*
     * Sensitivitas kamera.
     *
     * Nanti bisa disesuaikan
     * dengan touch input Levi.
     */
    float sensitivity{
        0.15f
    };



};





CameraState&
getCameraState()
noexcept;



}
