#pragma once

namespace levifreecam::camera
{

struct CameraState
{
    bool enabled = false;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float yaw = 0.0f;
    float pitch = 0.0f;
};


CameraState& getCameraState();

}
