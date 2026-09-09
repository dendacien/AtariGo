#pragma once

#include <optional>
#include <set>
#include <string_view>

#include "board.hpp"
#include "move_result.hpp"
#include "position.hpp"
#include "stone_color.hpp"

namespace atarigo {

// One round of Atari Go: owns the board, whose turn it is and the winner.
class Game {
public:
    Game() : board_(Board::kDefaultSize) {}

    MoveResult tryPlacement(Position stone);

    StoneColor currentTurn() const { return turn_; }
    StoneColor winner() const { return winner_; }
    bool isRunning() const { return running_; }
    void endGame() { running_ = false; }

    // The player to move gives up; their opponent takes the win.
    void currentPlayerConcedes();

    std::optional<Position> suggestMoveForCurrentPlayer() { return board_.suggestMove(turn_); }

    int boardSize() const { return board_.size(); }
    std::string_view intersectionSymbol(Position stone) const { return symbolOf(board_.at(stone)); }
    const std::set<Position>& captures() const { return board_.capturedStones(); }
    const std::set<Position>& atariPositions() const { return board_.atariStones(); }

private:
    Board board_;
    bool running_ = true;
    StoneColor turn_ = StoneColor::Black;
    StoneColor winner_ = StoneColor::Empty;
};

}  // namespace atarigo