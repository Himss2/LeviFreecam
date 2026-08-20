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

constexpr char
    kLogTag[] =
        "Levi Freecam";


/*
 * =========================================================
 * Minecraft library
 * =========================================================
 */
constexpr std::string_view
    kMinecraftLibrary =
        "libminecraftpe.so";


/*
 * =========================================================
 * LoopbackPacketSender::sendToServer
 * =========================================================
 *
 * Signature taken from the BedrockTools source supplied
 * for this project.
 *
 * If Minecraft changes this function in a future update,
 * this is one of the signatures we will update.
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


/*
 * =========================================================
 * Bedrock packet IDs
 * =========================================================
 */
constexpr std::uint32_t
    kPlayerAuthInputPacketId =
        144;


/*
 * =========================================================
 * Packet virtual table
 * =========================================================
 *
 * Minecraft Packet begins with a normal C++ vtable.
 *
 * Packet declares:
 *
 * virtual ~Packet()
 * virtual MinecraftPacketIds getId() const
 *
 * Android uses the Itanium C++ ABI.
 *
 * The destructor occupies:
 *
 * vtable[0]
 * vtable[1]
 *
 * therefore:
 *
 * Packet::getId()
 *
 * is:
 *
 * vtable[2]
 */
constexpr std::size_t
    kPacketGetIdVtableIndex =
        2;


/*
 * =========================================================
 * Function types
 * =========================================================
 */
using SendToServerFn =
    void (*)(
        void* sender,
        void* packet
    );


using PacketGetIdFn =
    std::uint32_t (*)(
        const void* packet
    );


/*
 * Original Minecraft function.
 */
SendToServerFn
    gOriginalSendToServer =
        nullptr;


/*
 * Original target address.
 */
void*
    gHookTarget =
        nullptr;


/*
 * We only print one debug message per hook install
 * when PlayerAuthInput is first detected.
 */
std::atomic_bool
    gLoggedPlayerAuthInput{
        false
    };


/*
 * =========================================================
 * Packet ID
 * =========================================================
 */
std::uint32_t getPacketId(
    const void* packet
) noexcept {

    if (
        packet == nullptr
    ) {
        return 0;
    }


    /*
     * Object begins with vtable pointer.
     */
    void** vtable =
        *reinterpret_cast<void***>(
            const_cast<void*>(
                packet
            )
        );


    if (
        vtable == nullptr
    ) {
        return 0;
    }


    /*
     * Packet::getId()
     */
    void* entry =
        vtable[
            kPacketGetIdVtableIndex
        ];


    if (
        entry == nullptr
    ) {
        return 0;
    }


    const auto getId =
        reinterpret_cast<
            PacketGetIdFn
        >(
            entry
        );


    return getId(
        packet
    );
}


/*
 * =========================================================
 * sendToServer HOOK
 * =========================================================
 */
void sendToServerDetour(
    void* sender,
    void* packet
) {

    /*
     * Always keep original available.
     */
    const auto original =
        gOriginalSendToServer;


    if (
        original == nullptr
    ) {
        return;
    }


    /*
     * Determine packet ID.
     */
    const std::uint32_t packetId =
        getPacketId(
            packet
        );


    /*
     * =====================================================
     * PlayerAuthInput
     * =====================================================
     */
    if (
        packetId ==
        kPlayerAuthInputPacketId
    ) {

        auto& controller =
            FreecamController::
                instance();


        /*
         * Count packet.
         */
        controller.
            notePlayerAuthInput();


        /*
         * Log only first detected packet.
         *
         * This proves our hook + getId are working.
         */
        bool expected =
            false;


        if (
            gLoggedPlayerAuthInput.
                compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_acq_rel
                )
        ) {

            __android_log_print(
                ANDROID_LOG_INFO,
                kLogTag,

                "PlayerAuthInput detected "
                "through sendToServer hook "
                "(id=%u)",

                packetId
            );
        }


        /*
         * =================================================
         * IMPORTANT
         * =================================================
         *
         * Ambient Freecam blocks PlayerAuthInput.
         *
         * But we MUST NOT block it yet.
         *
         * First we need:
         *
         * Local Spectator = ACTIVE
         *
         * Then:
         *
         * spectatorApplied = true
         *
         * Only after that is PlayerAuthInput
         * allowed to be dropped.
         *
         * This prevents:
         *
         * - rubberband
         * - broken normal movement
         * - movement desync
         *
         */
        if (
            controller.
                shouldSuppressPlayerAuthInput()
        ) {

            /*
             * DROP PACKET.
             *
             * This branch will remain unreachable
             * in v0.2.0 because spectatorApplied
             * is still false.
             */
            return;
        }
    }


    /*
     * =====================================================
     * NORMAL MINECRAFT PATH
     * =====================================================
     */
    original(
        sender,
        packet
    );
}

} // namespace


/*
 * =========================================================
 * INSTALL
 * =========================================================
 */
bool PacketHook::install() {

    if (
        mInstalled
    ) {
        return true;
    }


    /*
     * Resolve:
     *
     * LoopbackPacketSender::sendToServer
     */
    const std::uintptr_t
        targetAddress =

            pl::memory::
                resolveSignature(

                    kSendToServerSignature,

                    kMinecraftLibrary
                );


    if (
        targetAddress == 0
    ) {
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


    /*
     * Install Levi Preloader hook.
     */
    const int result =

        pl::memory::hook(

            target,

            detour,

            &original,

            pl::memory::
                HookPriority::Normal
        );


    if (
        result != 0 ||
        original == nullptr
    ) {

        return false;
    }


    /*
     * Save original function.
     */
    gOriginalSendToServer =
        reinterpret_cast<
            SendToServerFn
        >(
            original
        );


    gHookTarget =
        target;


    gLoggedPlayerAuthInput.store(
        false,
        std::memory_order_release
    );


    mTargetAddress =
        targetAddress;


    mInstalled =
        true;


    return true;
}


/*
 * =========================================================
 * UNINSTALL
 * =========================================================
 */
void PacketHook::uninstall()
    noexcept {

    if (
        !mInstalled
    ) {
        return;
    }


    if (
        gHookTarget != nullptr
    ) {

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
        false,
        std::memory_order_release
    );


    mTargetAddress =
        0;


    mInstalled =
        false;
}


/*
 * =========================================================
 * STATUS
 * =========================================================
 */
bool PacketHook::installed()
    const noexcept {

    return mInstalled;
}


std::uintptr_t
PacketHook::targetAddress()
    const noexcept {

    return mTargetAddress;
}

} // namespace levifreecam::hooks
