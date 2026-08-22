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






/*
 *
 * LoopbackPacketSender::sendToServer
 *
 * Digunakan untuk intercept packet
 * sebelum dikirim ke server.
 *
 */
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


    if(
        packet == nullptr
    )
    {
        return 0;
    }






    void** vtable =

        *reinterpret_cast<void***>(

            const_cast<void*>(

                packet

            )

        );






    if(
        vtable == nullptr
    )
    {
        return 0;
    }






    void*
    entry =

        vtable[

            kPacketGetIdVtableIndex

        ];






    if(
        entry == nullptr
    )
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






    if(
        original == nullptr
    )
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








        controller.

            notePlayerAuthInput();








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
         * Native Freecam input isolation
         *
         *
         * Saat kamera aktif:
         *
         * - Camera Entity tetap menerima kontrol
         * - LocalPlayer tidak menerima
         *   input movement
         *
         */


        if(

            controller.

            shouldSuppressPlayerAuthInput()

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
