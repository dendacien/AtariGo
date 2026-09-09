#pragma once

#include <string>
#include <utility>

#include "stone_color.hpp"

namespace atarigo {

// Outcome of attempting to place a single stone.
struct MoveResult {
    bool legal = false;
    bool capture = false;
    StoneColor winner = StoneColor::Empty;
    std::string message;

    static MoveResult illegal(std::string message) {
        return {false, false, StoneColor::Empty, std::move(message)};
    }

    static MoveResult placed(std::string message) {
        return {true, false, StoneColor::Empty, std::move(message)};
    }

    static MoveResult capturing(StoneColor winner) {
        return {true, true, winner, "Capture made. Game over."};
    }
};

}  // namespace atarigo