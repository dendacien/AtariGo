#pragma once

#include <string_view>

namespace atarigo {

enum class StoneColor { Empty, Black, White };

constexpr StoneColor opponentOf(StoneColor color) {
    switch (color) {
        case StoneColor::Black: return StoneColor::White;
        case StoneColor::White: return StoneColor::Black;
        default: return StoneColor::Empty;
    }
}

constexpr bool isPlayerColor(StoneColor color) {
    return color == StoneColor::Black || color == StoneColor::White;
}

constexpr std::string_view symbolOf(StoneColor color) {
    switch (color) {
        case StoneColor::Black: return "(B)";
        case StoneColor::White: return "(W)";
        default: return "   ";
    }
}

constexpr std::string_view nameOf(StoneColor color) {
    switch (color) {
        case StoneColor::Black: return "black";
        case StoneColor::White: return "white";
        default: return "empty";
    }
}

}  // namespace atarigo