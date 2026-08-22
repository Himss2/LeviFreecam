#include "camera/NativeCameraController.hpp"

#include "camera/CameraController.hpp"


#include <android/log.h>



namespace levifreecam::camera {



namespace {


constexpr char kLogTag[] =
    "Levi Freecam";


}







NativeCameraController&
NativeCameraController::instance()
noexcept
{

    static NativeCameraController controller;

    return controller;

}









bool NativeCameraController::enable()
noexcept
{

    auto& state =
        getCameraState();



    if(
        state.enabled.load()
    )
    {
        return true;
    }




    /*
     * Request capture kamera asli
     */
    state.captureRequested.store(
        true
    );


    state.captured.store(
        false
    );





    /*
     * Reset velocity
     */
    state.velocity.x = 0.0f;

    state.velocity.y = 0.0f;

    state.velocity.z = 0.0f;





    /*
     * Detach camera dari LocalPlayer.
     *
     * Setelah aktif:
     *
     * vanilla camera update
     * tidak boleh mengambil
     * kembali kontrol kamera.
     */
    state.detached.store(
        true
    );




    state.enabled.store(
        true
    );





    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Native camera enabled"
    );


    __android_log_print(
        ANDROID_LOG_INFO,
        kLogTag,
        "Camera detached from player"
    );



    return true;

}









bool NativeCameraController::disable()
noexcept
{

    auto& state =
        getCameraState();





    void* camera =
        mLastCameraComponent.load();





    if(camera)
    {


        CameraController::
        instance()
        .setTransform(
            camera,
            state.originalPosition,
            state.originalOrientation
        );




        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Camera restored"
        );

    }







    mLastCameraComponent.store(
        nullptr
    );





    state.enabled.store(
        false
    );



    state.captured.store(
        false
    );



    state.captureRequested.store(
        false
    );




    /*
     * Kembalikan kamera
     * ke mode vanilla.
     */
    state.detached.store(
        false
    );





    state.velocity.x = 0.0f;

    state.velocity.y = 0.0f;

    state.velocity.z = 0.0f;





    return true;

}









bool NativeCameraController::isEnabled()
const noexcept
{

    return getCameraState()
        .enabled
        .load();

}









bool NativeCameraController::cameraCaptured()
const noexcept
{

    return getCameraState()
        .captured
        .load();

}









void NativeCameraController::requestRecapture()
noexcept
{

    getCameraState()
        .captureRequested
        .store(
            true
        );

}









void NativeCameraController::onCameraTransform(
    void* cameraComponent
)
noexcept
{

    if(
        cameraComponent == nullptr
    )
    {
        return;
    }





    auto& state =
        getCameraState();





    if(
        !state.enabled.load()
    )
    {
        return;
    }





    mLastCameraComponent.store(
        cameraComponent
    );









    /*
     * Capture posisi camera pertama kali
     */
    if(
        state.captureRequested.load()
    )
    {



        Vec3 pos{};

        CameraOrientation rot{};





        bool posOK =
            CameraController::
            instance()
            .readPosition(
                cameraComponent,
                pos
            );





        bool rotOK =
            CameraController::
            instance()
            .readOrientation(
                cameraComponent,
                rot
            );







        if(posOK)
        {

            state.originalPosition =
                pos;



            state.position =
                pos;

        }







        if(rotOK)
        {

            state.originalOrientation =
                rot;



            state.orientation =
                rot;

        }







        state.velocity.x = 0.0f;

        state.velocity.y = 0.0f;

        state.velocity.z = 0.0f;






        state.captureRequested.store(
            false
        );



        state.captured.store(
            true
        );







        __android_log_print(
            ANDROID_LOG_INFO,
            kLogTag,
            "Camera captured"
        );

    }









    /*
     * Freecam takeover.
     *
     * Semua transform kamera
     * berasal dari virtual camera.
     */
    if(
        state.captured.load()
    )
    {


        CameraController::
        instance()
        .setTransform(
            cameraComponent,
            state.position,
            state.orientation
        );






        /*
         * Lock tambahan.
         *
         * Mencegah Minecraft
         * menulis ulang posisi
         * player asli.
         */
        if(
            state.detached.load()
        )
        {

            CameraController::
            instance()
            .writePosition(
                cameraComponent,
                state.position
            );

        }


    }



}









void NativeCameraController::update()
noexcept
{

    auto& state =
        getCameraState();





    if(
        !state.enabled.load()
    )
    {
        return;
    }






    if(
        !state.captured.load()
    )
    {
        return;
    }






    /*
     * Movement sementara masih
     * menunggu input hook.
     *
     * Tidak ada auto movement.
     */



    return;


}









void NativeCameraController::translate(
    float x,
    float y,
    float z
)
noexcept
{

    auto& state =
        getCameraState();





    if(
        !state.enabled.load()
    )
    {
        return;
    }






    state.velocity.x = x;

    state.velocity.y = y;

    state.velocity.z = z;



}









void NativeCameraController::rotate(
    float yawDelta,
    float pitchDelta
)
noexcept
{

    auto& state =
        getCameraState();





    if(
        !state.enabled.load()
    )
    {
        return;
    }





    state.yaw +=
        yawDelta *
        state.sensitivity;



    state.pitch +=
        pitchDelta *
        state.sensitivity;






}







}
