#pragma once

#include "camera/CameraState.hpp"

#include <cstddef>


namespace levifreecam::camera {


class CameraController final {

public:


    static CameraController&
    instance() noexcept;



    bool readPosition(
        const void* cameraComponent,
        Vec3& out
    ) const noexcept;



    bool writePosition(
        void* cameraComponent,
        const Vec3& position
    ) const noexcept;



    bool applyOffset(
        void* cameraComponent,
        const Vec3& offset
    ) const noexcept;



    bool readOrientation(
        const void* cameraComponent,
        CameraOrientation& out
    ) const noexcept;



    bool writeOrientation(
        void* cameraComponent,
        const CameraOrientation& orientation
    ) const noexcept;



    /*
     * Copy seluruh transform kamera
     *
     * Dipakai untuk:
     * - backup camera asli
     * - restore camera setelah freecam off
     */
    bool copyTransform(
        void* destination,
        const void* source
    ) const noexcept;



    /*
     * Set posisi dan rotasi kamera sekaligus
     *
     * Dipakai oleh NativeCameraController
     * saat freecam aktif
     */
    bool setTransform(
        void* cameraComponent,
        const Vec3& position,
        const CameraOrientation& orientation
    ) const noexcept;



    static constexpr std::size_t
        kOrientationOffset =
            0x28;



    static constexpr std::size_t
        kPositionOffset =
            0x38;



    static constexpr std::size_t
        kCameraComponentStride =
            0x118;



private:


    CameraController() = default;


};


}
