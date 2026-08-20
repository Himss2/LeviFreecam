#pragma once

#include "game/GameTypes.hpp"

#include <cstdint>
#include <optional>

namespace levifreecam::game {

class GameModeController final {
public:
    static GameModeController& instance();

    bool resolve() noexcept;

    void clear() noexcept;

    [[nodiscard]]
    bool available() const noexcept;

    [[nodiscard]]
    std::optional<std::int32_t>
    getLocalGameType(
        const void* localPlayer
    ) const noexcept;

    bool setLocalGameType(
        void* localPlayer,
        std::int32_t gameType
    ) const noexcept;

    bool setLocalGameType(
        void* localPlayer,
        GameType gameType
    ) const noexcept;

    [[nodiscard]]
    std::uintptr_t getterAddress()
        const noexcept;

    [[nodiscard]]
    std::uintptr_t setterAddress()
        const noexcept;

private:
    GameModeController() = default;

    using GetPlayerGameTypeFn =
        std::int32_t (*)(
            const void* player
        );

    using SetLocalGameTypeFn =
        void (*)(
            void* localPlayer,
            std::int32_t gameType
        );

    GetPlayerGameTypeFn
        mGetPlayerGameType{nullptr};

    SetLocalGameTypeFn
        mSetLocalGameType{nullptr};

    std::uintptr_t
        mGetterAddress{0};

    std::uintptr_t
        mSetterAddress{0};
};

} // namespace levifreecam::game
