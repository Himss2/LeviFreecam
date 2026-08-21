#include "camera/CameraController.hpp"


#include <android/log.h>


#include <cstddef>
#include <cstring>
#include <cstdint>



namespace levifreecam::camera {



namespace {


constexpr char kLogTag[] =
    "Levi Freecam";



bool isValidPointer(
    const void* ptr
) noexcept
{

    if(ptr == nullptr)
        return false;



    const auto address =
        reinterpret_cast<
            std::uintptr_t
        >(
            ptr
        );



    /*
     * Pointer minimum alignment check
     */

    if(address < 0x10000)
        return false;



    return true;

}



}





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


    if(
        !isValidPointer(
            cameraComponent
        )
    )
    {

        return false;

    }




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


    if(
        !isValidPointer(
            cameraComponent
        )
    )
    {

        return false;

    }





    auto* base =

        static_cast<
            std::byte*
        >(
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


    if(
        !isValidPointer(
            cameraComponent
        )
    )
    {

        return false;

    }





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


    if(
        !isValidPointer(
            cameraComponent
        )
    )
    {

        return false;

    }






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


    if(
        !isValidPointer(
            cameraComponent
        )
    )
    {

        return false;

    }







    auto* base =


        static_cast<
            std::byte*
        >(
            cameraComponent
        );







    std::memcpy(

        base + kOrientationOffset,

        &orientation,

        sizeof(CameraOrientation)

    );







    return true;


}








bool CameraController::copyTransform(
    void* destination,
    const void* source
) const noexcept
{


    if(
        !isValidPointer(destination)
        ||
        !isValidPointer(source)
    )
    {

        return false;

    }






    Vec3 position{};


    CameraOrientation orientation{};






    if(
        !readPosition(
            source,
            position
        )
    )
    {

        return false;

    }







    if(
        !readOrientation(
            source,
            orientation
        )
    )
    {

        return false;

    }







    writePosition(

        destination,

        position

    );







    writeOrientation(

        destination,

        orientation

    );







    return true;


}








bool CameraController::setTransform(

    void* cameraComponent,

    const Vec3& position,

    const CameraOrientation& orientation

) const noexcept

{


    if(
        !writePosition(

            cameraComponent,

            position

        )
    )
    {

        return false;

    }






    if(
        !writeOrientation(

            cameraComponent,

            orientation

        )
    )
    {

        return false;

    }







    return true;


}



}
