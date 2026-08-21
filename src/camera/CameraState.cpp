#include "CameraState.hpp"


namespace levifreecam::camera {


namespace {

CameraState gCameraState{};

}


CameraState&
getCameraState()
noexcept {

    return gCameraState;

}


}
