#include "game/GameModeController.hpp"

#include <dlfcn.h>

#include <array>
#include <cstddef>

namespace levifreecam::game {

namespace {

constexpr char kMinecraftLibrary[] =
    "libminecraftpe.so";

/*
 * LocalPlayer::
 * setPlayerGameTypeWithoutServerNotification(
 *     GameType
 * )
 *
 * Itanium ARM64 C++ ABI symbol.
 *
 * Fungsi ini dipilih karena tidak boleh
 * mengirim perubahan gamemode ke server.
 */
constexpr char kSetLocalGameTypeSymbol[] =
    "_ZN11LocalPlayer42"
    "setPlayerGameTypeWithoutServerNotification"
    "E8GameType";

/*
 * Player::getPlayerGameType() const
 */
constexpr char kGetPlayerGameTypeConstSymbol[] =
    "_ZNK6Player17getPlayerGameTypeEv";

/*
 * Fallback untuk build Minecraft yang
 * mengekspos getter tanpa qualifier const.
 */
constexpr char kGetPlayerGameTypeSymbol[] =
    "_ZN6Player17getPlayerGameTypeEv";


void* resolveLoadedSymbol(
    const char* symbol
) noexcept {

    if (symbol == nullptr) {
        return nullptr;
    }

    /*
     * Coba global namespace terlebih dahulu.
     */
    if (
        void* address =
            dlsym(
                RTLD_DEFAULT,
                symbol
            );
        address != nullptr
    ) {
        return address;
    }

    /*
     * Kemudian cari langsung pada
     * libminecraftpe.so yang sudah dimuat.
     */
    void* handle =
        dlopen(
            kMinecraftLibrary,
            RTLD_NOW | RTLD_NOLOAD
        );

    if (handle == nullptr) {
        return nullptr;
    }

    void* address =
        dlsym(
            handle,
            symbol
        );

    dlclose(handle);

    return address;
}

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
     * Local-only GameType setter.
     */
    void* setter =
        resolveLoadedSymbol(
            kSetLocalGameTypeSymbol
        );

    /*
     * Cari getter GameType.
     */
    const std::array<
        const char*,
        2
    > getterSymbols{
        kGetPlayerGameTypeConstSymbol,
        kGetPlayerGameTypeSymbol
    };

    void* getter = nullptr;

    for (
        const char* symbol :
        getterSymbols
    ) {

        getter =
            resolveLoadedSymbol(
                symbol
            );

        if (getter != nullptr) {
            break;
        }
    }

    /*
     * Jangan pernah memakai
     * LocalPlayer::setPlayerGameType()
     * sebagai fallback.
     *
     * Fungsi itu berpotensi memberi tahu
     * server dan akan merusak konsep Freecam.
     */
    if (
        setter == nullptr ||
        getter == nullptr
    ) {

        clear();

        return false;
    }

    mSetLocalGameType =
        reinterpret_cast<
            SetLocalGameTypeFn
        >(
            setter
        );

    mGetPlayerGameType =
        reinterpret_cast<
            GetPlayerGameTypeFn
        >(
            getter
        );

    mSetterAddress =
        reinterpret_cast<
            std::uintptr_t
        >(
            setter
        );

    mGetterAddress =
        reinterpret_cast<
            std::uintptr_t
        >(
            getter
        );

    return true;
}


void GameModeController::clear()
    noexcept {

    mGetPlayerGameType = nullptr;
    mSetLocalGameType = nullptr;

    mGetterAddress = 0;
    mSetterAddress = 0;
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
