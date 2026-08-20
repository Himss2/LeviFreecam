#include "game/GameModeController.hpp"

#include <pl/memory/Signature.hpp>

#include <cstdint>
#include <string_view>

namespace levifreecam::game {

namespace {

constexpr std::string_view kMinecraftLibrary =
    "libminecraftpe.so";

/*
 * ============================================================
 * Player GameType getter
 * ============================================================
 *
 * Resolved directly from the libminecraftpe.so supplied
 * for this project.
 *
 * Exact match in current binary:
 *
 *     0x0F0CB1B4
 *
 * This routine is called by Minecraft before and after
 * changing LocalPlayer GameType.
 *
 * Signature match count in supplied binary:
 *
 *     1
 */
constexpr std::string_view kGetPlayerGameTypeSignature =
    "FD 7B BE A9 "
    "F3 0B 00 F9 "
    "FD 03 00 91 "
    "F3 03 00 AA "
    "00 20 00 91 "
    "? ? ? ? "
    "1F 14 00 71 "
    "? ? ? ? "
    "E0 03 13 AA "
    "? ? ? ? "
    "08 00 40 F9 "
    "01 35 42 F9";

/*
 * ============================================================
 * LocalPlayer local-only GameType setter
 * ============================================================
 *
 * Resolved directly from the supplied Minecraft binary.
 *
 * Exact match:
 *
 *     0x0A649B7C
 *
 * Minecraft's client-side SetPlayerGameTypePacket handler
 * calls this routine with:
 *
 *     x0 = LocalPlayer*
 *     w1 = GameType
 *
 * It does not use the normal command/server gamemode path.
 *
 * This is the function we want for Freecam.
 *
 * Signature match count:
 *
 *     1
 */
constexpr std::string_view kSetLocalGameTypeSignature =
    "FD 7B BD A9 "
    "F5 0B 00 F9 "
    "F4 4F 02 A9 "
    "FD 03 00 91 "
    "F3 03 01 2A "
    "F4 03 00 AA "
    "? ? ? ? "
    "F5 03 00 2A "
    "E0 03 14 AA "
    "E1 03 13 2A "
    "? ? ? ? "
    "08 07 82 52 "
    "80 02 08 8B "
    "? ? ? ? "
    "E0 03 14 AA "
    "? ? ? ? "
    "E2 03 00 2A "
    "E0 03 14 AA "
    "E1 03 15 2A";

} // namespace


GameModeController&
GameModeController::instance() {

    static GameModeController controller;

    return controller;
}


bool GameModeController::resolve()
    noexcept {

    clear();

    /*
     * ========================================================
     * Resolve GameType getter
     * ========================================================
     */
    const std::uintptr_t getterAddress =
        pl::memory::resolveSignature(
            kGetPlayerGameTypeSignature,
            kMinecraftLibrary
        );

    if (getterAddress == 0) {
        clear();
        return false;
    }

    /*
     * ========================================================
     * Resolve local-only GameType setter
     * ========================================================
     */
    const std::uintptr_t setterAddress =
        pl::memory::resolveSignature(
            kSetLocalGameTypeSignature,
            kMinecraftLibrary
        );

    if (setterAddress == 0) {
        clear();
        return false;
    }

    /*
     * ========================================================
     * Convert addresses to callable functions
     * ========================================================
     */
    mGetPlayerGameType =
        reinterpret_cast<GetPlayerGameTypeFn>(
            getterAddress
        );

    mSetLocalGameType =
        reinterpret_cast<SetLocalGameTypeFn>(
            setterAddress
        );

    mGetterAddress =
        getterAddress;

    mSetterAddress =
        setterAddress;

    return true;
}


void GameModeController::clear()
    noexcept {

    mGetPlayerGameType =
        nullptr;

    mSetLocalGameType =
        nullptr;

    mGetterAddress =
        0;

    mSetterAddress =
        0;
}


bool GameModeController::available()
    const noexcept {

    return
        mGetPlayerGameType != nullptr &&
        mSetLocalGameType != nullptr;
}


std::optional<std::int32_t>
GameModeController::getLocalGameType(
    const void* localPlayer
) const noexcept {

    if (
        !available() ||
        localPlayer == nullptr
    ) {
        return std::nullopt;
    }

    return mGetPlayerGameType(
        localPlayer
    );
}


bool GameModeController::setLocalGameType(
    void* localPlayer,
    std::int32_t gameType
) const noexcept {

    if (
        !available() ||
        localPlayer == nullptr
    ) {
        return false;
    }

    mSetLocalGameType(
        localPlayer,
        gameType
    );

    return true;
}


bool GameModeController::setLocalGameType(
    void* localPlayer,
    GameType gameType
) const noexcept {

    return setLocalGameType(
        localPlayer,
        toInt(gameType)
    );
}


std::uintptr_t
GameModeController::getterAddress()
    const noexcept {

    return mGetterAddress;
}


std::uintptr_t
GameModeController::setterAddress()
    const noexcept {

    return mSetterAddress;
}

} // namespace levifreecam::game
