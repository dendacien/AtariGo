#pragma once

#include <string>
#include <tuple>

namespace atarigo {

struct Position {
    int row = 0;
    int col = 0;

    // Human readable label such as "A1" (row letter, column number).
    std::string label() const {
        return std::string(1, static_cast<char>('A' + row)) + std::to_string(col + 1);
    }

    friend bool operator==(const Position& lhs, const Position& rhs) {
        return lhs.row == rhs.row && lhs.col == rhs.col;
    }

    friend bool operator!=(const Position& lhs, const Position& rhs) { return !(lhs == rhs); }

    // Row major ordering so every set of positions iterates deterministically.
    friend bool operator<(const Position& lhs, const Position& rhs) {
        return std::tie(lhs.row, lhs.col) < std::tie(rhs.row, rhs.col);
    }
};

}  // namespace atarigo