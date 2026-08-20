#pragma once

#include <atomic>
#include <cstdint>

namespace levifreecam::camera {


enum class CameraPreset : uint32_t {

    FirstPerson = 0,

    ThirdPerson = 1,

    ThirdPersonFront = 2,

    Free = 3
};


class CameraController final {

public:

    static CameraController& instance();


    bool resolve();


    bool enableFreeCamera();


    bool disableFreeCamera();


    bool enabled() const noexcept;


    uintptr_t instructionAddress()
        const noexcept;



private:

    CameraController() = default;



    using CameraInstructionFn =
        void(*)(

            void* cameraSystem,

            CameraPreset preset

        );


    CameraInstructionFn
        mInstruction{nullptr};



    uintptr_t
        mAddress{0};



    std::atomic_bool
        mEnabled{false};

};


}
