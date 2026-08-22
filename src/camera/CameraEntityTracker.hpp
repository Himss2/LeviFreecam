#pragma once


namespace levifreecam::camera {


class CameraEntityTracker final
{

public:


    static CameraEntityTracker&
    instance()
    noexcept;



    void track(
        void* cameraComponent
    )
    noexcept;



private:

    CameraEntityTracker() = default;


};


}
