#include "match.hpp"

#include <stdexcept>
#include <utility>

namespace atarigo {

Match::Match(GameView& view, Player black, Player white)
    : view_(view), black_(std::move(black)), white_(std::move(white)) {}

const Player& Match::playerFor(StoneColor color) const {
    switch (color) {
        case StoneColor::Black: return black_;
        case StoneColor::White: return white_;
        default: throw std::invalid_argument("Empty has no player");
    }
}

void Match::run() {
    while (true) {
        game_ = Game();
        lastMove_.clear();
        playRound();
        announceResult();

        // A quit ends the match outright; only a real result offers a rematch.
        if (game_.winner() == StoneColor::Empty) return;
        if (!view_.askPlayAgain()) return;
    }
}

void Match::playRound() {
    while (game_.isRunning()) {
        const Player& current = playerFor(game_.currentTurn());
        if (current.isBot()) playBotTurn(current);
        else playHumanTurn(current);
    }
}

void Match::playHumanTurn(const Player& current) {
    // Wipe the previous player's view, then hand over the details of their move.
    view_.clearScreen();
    view_.showTurnBanner(current, lastMove_);
    view_.showBoard(game_);

    while (true) {
        const MoveRequest request = view_.askMove(current);
        if (request.kind == MoveRequest::Kind::Quit) {
            game_.endGame();
            return;
        }
        if (request.kind == MoveRequest::Kind::Invalid) {
            view_.showMessage(request.error);
            continue;
        }

        const MoveResult result = game_.tryPlacement(request.position);
        if (!result.legal) {
            view_.showMessage(result.message);
            continue;
        }

        recordMove(current, request.position, result);
        if (result.capture) finishWithCapture(result);
        return;
    }
}

void Match::playBotTurn(const Player& current) {
    const std::optional<Position> suggestion = game_.suggestMoveForCurrentPlayer();
    const MoveResult result = suggestion ? game_.tryPlacement(*suggestion) : MoveResult::illegal("");

    if (!result.legal) {
        view_.showMessage("The computer could not find a legal move. You win this round.");
        game_.currentPlayerConcedes();
        return;
    }

    recordMove(current, *suggestion, result);
    if (result.capture) finishWithCapture(result);
}

void Match::finishWithCapture(const MoveResult& result) {
    // The final board stays on screen: both players should see the capture.
    view_.showBoard(game_);
    view_.showMessage(result.message);
    game_.endGame();
}

void Match::recordMove(const Player& player, Position stone, const MoveResult& result) {
    const std::string detail = result.capture ? "captured a chain at " + stone.label() : result.message;
    lastMove_ = player.name() + " " + std::string(player.symbol()) + " " + detail;
}

void Match::announceResult() {
    const StoneColor winner = game_.winner();
    if (winner == black_.color()) {
        black_.recordWin();
        view_.showMessage(black_.name() + " wins!");
    } else if (winner == white_.color()) {
        white_.recordWin();
        view_.showMessage(white_.name() + " wins!");
    } else {
        view_.showMessage("No winner, no points awarded.");
    }
    view_.showScore(black_, white_);
}

}  // namespace atarigo