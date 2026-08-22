#include "hooks/PacketHook.hpp"

#include "core/FreecamController.hpp"

#include <android/log.h>

#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string_view>


namespace levifreecam::hooks {


namespace {


constexpr char kLogTag[] =
    "Levi Freecam";


constexpr std::string_view
kMinecraftLibrary =
    "libminecraftpe.so";



constexpr std::string_view
kSendToServerSignature =

    "? ? ? A9 "
    "? ? ? F9 "
    "? ? ? A9 "
    "? ? ? A9 "
    "? ? ? A9 "
    "FD 03 00 91 "
    "? ? ? D1 "
    "55 D0 3B D5 "
    "F4 03 00 AA "
    "F3 03 01 AA";




constexpr std::uint32_t
kPlayerAuthInputPacketId =
    144;




using SendToServerFn =
    void (*)(
        void* sender,
        void* packet
    );



using PacketGetIdFn =
    std::uint32_t (*)(
        const void* packet
    );




SendToServerFn
gOriginalSendToServer =
    nullptr;



void*
gHookTarget =
    nullptr;




std::atomic_bool
gLoggedPlayerAuthInput{
    false
};




constexpr std::size_t
kPacketGetIdVtableIndex =
    2;




std::uint32_t getPacketId(

    const void* packet

)
noexcept

{

    if(packet == nullptr)
    {
        return 0;
    }



    void** vtable =

        *reinterpret_cast<void***>(

            const_cast<void*>(

                packet

            )

        );



    if(vtable == nullptr)
    {
        return 0;
    }



    void* entry =

        vtable[
            kPacketGetIdVtableIndex
        ];



    if(entry == nullptr)
    {
        return 0;
    }



    auto fn =

        reinterpret_cast<PacketGetIdFn>(

            entry

        );



    return fn(packet);

}





void sendToServerDetour(

    void* sender,

    void* packet

)

{

    auto original =
        gOriginalSendToServer;



    if(original == nullptr)
    {
        return;
    }



    const auto packetId =

        getPacketId(

            packet

        );



    if(
        packetId ==
        kPlayerAuthInputPacketId
    )

    {

        auto& controller =

            FreecamController::

            instance();



        controller.notePlayerAuthInput();



        bool expected =
            false;



        if(
            gLoggedPlayerAuthInput
            .compare_exchange_strong(

                expected,

                true

            )
        )

        {

            __android_log_print(

                ANDROID_LOG_INFO,

                kLogTag,

                "PlayerAuthInput detected id=%u",

                packetId

            );

        }



        /*
         *
         * Future input isolation layer.
         *
         */

        if(

            controller
            .shouldSuppressPlayerAuthInput()

        )

        {

            return;

        }

    }



    original(

        sender,

        packet

    );

}



} // anonymous namespace





bool PacketHook::install()

{

    if(
        mInstalled
    )
    {
        return true;
    }



    const auto targetAddress =


        pl::memory::

        resolveSignature(

            kSendToServerSignature,

            kMinecraftLibrary

        );



    if(
        targetAddress == 0
    )
    {

        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "Failed resolve sendToServer"

        );


        return false;

    }




    void* original =
        nullptr;



    void* target =

        reinterpret_cast<void*>(

            targetAddress

        );



    void* detour =

        reinterpret_cast<void*>(

            &sendToServerDetour

        );



    const int result =


        pl::memory::hook(

            target,

            detour,

            &original,

            pl::memory::

            HookPriority::Normal

        );



    if(

        result != 0 ||

        original == nullptr

    )

    {

        __android_log_print(

            ANDROID_LOG_ERROR,

            kLogTag,

            "Packet hook failed %d",

            result

        );


        return false;

    }



    gOriginalSendToServer =


        reinterpret_cast<SendToServerFn>(

            original

        );



    gHookTarget =

        target;



    gLoggedPlayerAuthInput.store(

        false

    );



    mTargetAddress =

        targetAddress;



    mInstalled =

        true;



    __android_log_print(

        ANDROID_LOG_INFO,

        kLogTag,

        "Packet hook installed 0x%lx",

        static_cast<unsigned long>(targetAddress)

    );



    return true;

}





void PacketHook::uninstall()

noexcept

{

    if(
        !mInstalled
    )
    {
        return;
    }



    if(
        gHookTarget != nullptr
    )

    {

        pl::memory::unhook(

            gHookTarget,

            reinterpret_cast<void*>(

                &sendToServerDetour

            )

        );

    }



    gOriginalSendToServer =
        nullptr;



    gHookTarget =
        nullptr;



    gLoggedPlayerAuthInput.store(

        false

    );



    mTargetAddress =
        0;



    mInstalled =
        false;

}





bool PacketHook::installed()

const noexcept

{

    return mInstalled;

}





std::uintptr_t

PacketHook::targetAddress()

const noexcept

{

    return mTargetAddress;

}



} // namespace levifreecam::hooks
