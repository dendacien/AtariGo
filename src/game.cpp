#include "game.hpp"

namespace atarigo {

MoveResult Game::tryPlacement(Position stone) {
    if (!running_) return MoveResult::illegal("Game is already over.");

    MoveResult result = board_.placeStone(stone, turn_);
    if (!result.legal) return result;

    if (result.capture) {
        winner_ = result.winner;
        return result;
    }

    turn_ = opponentOf(turn_);
    return result;
}

void Game::currentPlayerConcedes() {
    winner_ = opponentOf(turn_);
    endGame();
}

}  // namespace atarigo