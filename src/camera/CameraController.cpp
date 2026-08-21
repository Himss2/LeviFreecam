#include "camera/CameraController.hpp"


#include <cstddef>
#include <cstring>



namespace levifreecam::camera {



CameraController&
CameraController::instance()
noexcept
{

    static CameraController controller;

    return controller;

}




bool CameraController::readPosition(
    const void* cameraComponent,
    Vec3& out
) const noexcept
{


    if(cameraComponent == nullptr)
        return false;



    const auto* base =
        static_cast<
            const std::byte*
        >(
            cameraComponent
        );



    std::memcpy(
        &out,
        base + kPositionOffset,
        sizeof(Vec3)
    );


    return true;

}




bool CameraController::writePosition(
    void* cameraComponent,
    const Vec3& position
) const noexcept
{


    if(cameraComponent == nullptr)
        return false;



    auto* base =
        static_cast<std::byte*>(
            cameraComponent
        );



    std::memcpy(
        base + kPositionOffset,
        &position,
        sizeof(Vec3)
    );



    return true;

}




bool CameraController::applyOffset(
    void* cameraComponent,
    const Vec3& offset
) const noexcept
{


    if(cameraComponent == nullptr)
        return false;



    Vec3 current{};



    if(
        !readPosition(
            cameraComponent,
            current
        )
    )
    {
        return false;
    }



    current.x += offset.x;
    current.y += offset.y;
    current.z += offset.z;



    return writePosition(
        cameraComponent,
        current
    );

}




bool CameraController::readOrientation(
    const void* cameraComponent,
    CameraOrientation& out
) const noexcept
{


    if(cameraComponent == nullptr)
        return false;



    const auto* base =
        static_cast<
            const std::byte*
        >(
            cameraComponent
        );



    std::memcpy(
        &out,
        base + kOrientationOffset,
        sizeof(CameraOrientation)
    );



    return true;

}




bool CameraController::writeOrientation(
    void* cameraComponent,
    const CameraOrientation& orientation
) const noexcept
{


    if(cameraComponent == nullptr)
        return false;



    auto* base =
        static_cast<std::byte*>(
            cameraComponent
        );



    std::memcpy(
        base + kOrientationOffset,
        &orientation,
        sizeof(CameraOrientation)
    );



    return true;

}



}
