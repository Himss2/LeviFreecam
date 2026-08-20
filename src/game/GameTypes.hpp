#pragma once

#include <cstdint>

namespace levifreecam::game {

enum class GameType : std::int32_t {
    Survival = 0,
    Creative = 1,
    Adventure = 2,
    Spectator = 6
};

[[nodiscard]]
constexpr std::int32_t toInt(
    GameType type
) noexcept {
    return static_cast<std::int32_t>(type);
}

} // namespace levifreecam::game
