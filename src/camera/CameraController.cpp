#include "camera/CameraController.hpp"

#include <pl/memory/Signature.hpp>

#include <string_view>


namespace levifreecam::camera {


namespace {


constexpr std::string_view
kMinecraftLibrary =
"libminecraftpe.so";



/*
    CameraInstructionSystem

    Dari libminecraftpe.so Minecraft kita.

    Fungsi ini akan kita validasi
    kembali setelah build pertama.

*/

constexpr std::string_view
kCameraInstructionSignature =

"? ? ? A9 "
"? ? ? F9 "
"? ? ? A9 "
"? ? ? A9 "
"FD 03 00 91 "
"F4 03 00 AA "
"F5 03 01 AA "
"? ? ? ? "
"? ? ? ? ";


}



CameraController&
CameraController::instance()

{

    static CameraController controller;

    return controller;

}



bool CameraController::resolve()

{


    const uintptr_t address =

        pl::memory::resolveSignature(

            kCameraInstructionSignature,

            kMinecraftLibrary

        );



    if(address == 0)

    {

        return false;

    }



    mAddress = address;



    mInstruction =

        reinterpret_cast<CameraInstructionFn>(

            address

        );



    return true;

}





bool CameraController::enableFreeCamera()

{


    if(!mInstruction)

    {

        return false;

    }



    /*
        Untuk tahap pertama kita belum
        mengirim CameraInstructionPacket.

        Kita hanya memastikan jalur
        CameraInstruction System
        berhasil dipanggil.

    */


    mInstruction(

        nullptr,

        CameraPreset::Free

    );



    mEnabled.store(

        true,

        std::memory_order_release

    );



    return true;

}





bool CameraController::disableFreeCamera()

{


    if(!mInstruction)

    {

        return false;

    }



    mInstruction(

        nullptr,

        CameraPreset::FirstPerson

    );



    mEnabled.store(

        false,

        std::memory_order_release

    );



    return true;

}





bool CameraController::enabled()

const noexcept

{

    return mEnabled.load(

        std::memory_order_acquire

    );

}





uintptr_t CameraController::instructionAddress()

const noexcept

{

    return mAddress;

}



}
