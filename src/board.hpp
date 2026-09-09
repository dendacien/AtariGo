#pragma once

#include <optional>
#include <set>
#include <vector>
#include <stdexcept>

#include "chain.hpp"
#include "move_result.hpp"
#include "position.hpp"
#include "stone_color.hpp"

namespace atarigo {

// The playing grid plus the Atari Go capture rules.
class Board {
public:
    static constexpr int kDefaultSize = 9;

    explicit Board(int size = kDefaultSize);

    // Places a stone for `player`, reporting whether the move was legal and
    // whether it captured an enemy chain (which wins the game in Atari Go).
    MoveResult placeStone(Position stone, StoneColor player);

    // Picks a move for `player`: escape atari, capture, threaten, else drift.
    // Returns nothing when the board offers no legal move at all.
    std::optional<Position> suggestMove(StoneColor player);

    int size() const { return size_; }
    
    StoneColor at(Position stone) const;
    bool isOnBoard(Position stone) const;
    bool isEmpty(Position stone) const;

    const std::set<Position>& capturedStones() const { return captured_; }
    const std::set<Position>& atariStones() const { return atari_; }

private:
    std::size_t index(Position stone) const {
        if (!isOnBoard(stone)) {
            throw std::out_of_range("position " + stone.label() + " is off the board");
        }
        return static_cast<std::size_t>(stone.row) * static_cast<std::size_t>(size_) +
               static_cast<std::size_t>(stone.col);
    }

    StoneColor stoneAt(Position stone) const { return grid_[index(stone)]; }

    void setStone(Position stone, StoneColor color) { grid_[index(stone)] = color; }

    // Legality can depend on a capture, so the cheapest check is to play the
    // move on a snapshot of the board state and roll it back.
    bool canPlaceStone(Position candidate, StoneColor player);

    
    Chain findChain(Position start, StoneColor color) const;
    std::vector<Position> orthogonalNeighbors(Position start) const;
    std::vector<Position> orthogonalEnemyNeighbors(Position origin, StoneColor player) const;

    int size_;
    std::vector<StoneColor> grid_;
    std::set<Position> captured_;
    std::set<Position> atari_;
};

}  // namespace atarigo