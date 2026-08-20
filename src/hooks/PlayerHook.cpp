#include "hooks/PlayerHook.hpp"

#include "core/FreecamController.hpp"

#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>

#include <cstdint>
#include <string_view>

namespace levifreecam::hooks {

namespace {

constexpr std::string_view
    kMinecraftLibrary =
        "libminecraftpe.so";

/*
 * LocalPlayer normalTick signature.
 *
 * Ini berasal dari implementasi BedrockTools
 * yang sudah kita gunakan sebagai referensi.
 *
 * Argumen pertama adalah pointer LocalPlayer.
 */
constexpr std::string_view
    kNormalTickSignature =

        "? ? ? FC "
        "? ? ? A9 "
        "? ? ? A9 "
        "? ? ? A9 "
        "? ? ? A9 "
        "? ? ? A9 "
        "? ? ? A9 "
        "? ? ? 91 "
        "? ? ? D1 "
        "54 D0 3B D5 "
        "F3 03 00 AA "
        "? ? ? F9 "
        "? ? ? F8 "
        "? ? ? 39";


using NormalTickFn =
    void (*)(
        void* localPlayer
    );


NormalTickFn
    gOriginalNormalTick =
        nullptr;


void*
    gHookTarget =
        nullptr;


/*
 * Hook LocalPlayer tick.
 */
void normalTickDetour(
    void* localPlayer
) {

    const auto original =
        gOriginalNormalTick;

    /*
     * Jalankan Minecraft terlebih dahulu.
     */
    if (original != nullptr) {
        original(
            localPlayer
        );
    }

    /*
     * Baru proses Freecam.
     *
     * Dengan cara ini perubahan gamemode
     * terjadi di game tick thread, bukan
     * langsung dari callback UI CAM.
     */
    FreecamController::
        instance().
        onLocalPlayerTick(
            localPlayer
        );
}

} // namespace


bool PlayerHook::install() {

    if (mInstalled) {
        return true;
    }

    /*
     * Resolve LocalPlayer normalTick.
     */
    const std::uintptr_t
        targetAddress =

            pl::memory::
                resolveSignature(

                    kNormalTickSignature,

                    kMinecraftLibrary
                );

    if (targetAddress == 0) {
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
            &normalTickDetour
        );

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

    gOriginalNormalTick =
        reinterpret_cast<
            NormalTickFn
        >(
            original
        );

    gHookTarget =
        target;

    mTargetAddress =
        targetAddress;

    mInstalled =
        true;

    return true;
}


void PlayerHook::uninstall()
    noexcept {

    if (!mInstalled) {
        return;
    }

    if (
        gHookTarget != nullptr
    ) {

        pl::memory::unhook(

            gHookTarget,

            reinterpret_cast<void*>(
                &normalTickDetour
            )
        );
    }

    gOriginalNormalTick =
        nullptr;

    gHookTarget =
        nullptr;

    mTargetAddress =
        0;

    mInstalled =
        false;
}


bool PlayerHook::installed()
    const noexcept {

    return mInstalled;
}


std::uintptr_t
PlayerHook::targetAddress()
    const noexcept {

    return mTargetAddress;
}

} // namespace levifreecam::hooks
