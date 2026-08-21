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
     * Posisi virtual camera.
     */
    Vec3 position{};



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
     * Rotasi kamera.
     */
    CameraOrientation
        orientation{};

};



CameraState&
getCameraState()
noexcept;



}
