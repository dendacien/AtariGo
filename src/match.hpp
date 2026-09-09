#pragma once

#include <string>

#include "game.hpp"
#include "game_view.hpp"
#include "player.hpp"

namespace atarigo {

// Drives rounds of Atari Go between two players, keeping score.
// All presentation goes through GameView, so this class works unchanged for a
// console or a graphical front end.
class Match {
public:
    Match(GameView& view, Player black, Player white);

    // Plays rounds until a player quits or declines another round.
    void run();

private:
    void playRound();
    void playHumanTurn(const Player& current);
    void playBotTurn(const Player& current);
    void finishWithCapture(const MoveResult& result);
    void recordMove(const Player& player, Position stone, const MoveResult& result);
    void announceResult();

    const Player& playerFor(StoneColor color) const;

    GameView& view_;
    Player black_;
    Player white_;
    Game game_;
    // Description of the move just played, shown to the next player after the
    // screen is cleared.
    std::string lastMove_;
};

}  // namespace atarigo