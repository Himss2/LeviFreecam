#pragma once

#include <cstdint>

namespace levifreecam::camera
{

class NativeCameraController
{
public:

    static NativeCameraController& instance();


    bool resolve();

    bool enable();

    bool disable();


    bool isEnabled() const;


    void update();


private:

    NativeCameraController() = default;


private:

    bool mResolved = false;

    bool mEnabled = false;

    uintptr_t mCameraInstructionTick = 0;
};


}
