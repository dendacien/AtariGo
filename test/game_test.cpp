#include <deque>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "board.hpp"
#include "console_view.hpp"
#include "game.hpp"
#include "game_view.hpp"
#include "match.hpp"
#include "player.hpp"
#include "position.hpp"
#include "stone_color.hpp"

using namespace atarigo;

namespace {

int checks = 0;
int failures = 0;

void check(bool condition, const std::string& message) {
    ++checks;
    if (!condition) {
        ++failures;
        std::cout << "FAIL: " << message << "\n";
    }
}

void checkFalse(bool condition, const std::string& message) { check(!condition, message); }

template <typename T, typename U>
void checkEquals(const T& expected, const U& actual, const std::string& message) {
    ++checks;
    if (!(expected == actual)) {
        ++failures;
        std::cout << "FAIL: " << message << "\n";
    }
}

// ---------------------------------------------------------------- board rules

void rejectsInvalidAndOccupiedMoves() {
    Board board(9);

    checkFalse(board.placeStone({-1, 0}, StoneColor::Black).legal, "out-of-bounds moves must be illegal");
    checkFalse(board.placeStone({3, 4}, StoneColor::Black).legal, "occupied positions must be illegal");
}

void switchesTurnsAfterLegalMove() {
    Game game;

    const MoveResult result = game.tryPlacement({0, 0});

    check(result.legal, "an empty corner should accept a move");
    checkFalse(result.capture, "the opening move should not capture");
    check(game.currentTurn() == StoneColor::White, "a legal move should switch turns");
}

void rejectsMovesAfterGameEnds() {
    Game game;
    game.endGame();

    const MoveResult result = game.tryPlacement({0, 0});

    checkFalse(result.legal, "ended games must reject moves");
    checkEquals(std::string("Game is already over."), result.message,
                "ended-game message should explain the rejection");
}

void recordsCaptures() {
    Board board(9);

    check(board.placeStone({2, 4}, StoneColor::Black).legal, "setup move should be legal");
    const MoveResult capture = board.placeStone({3, 3}, StoneColor::Black);

    check(capture.legal, "capture move should be legal");
    check(capture.capture, "surrounding a chain's last liberty should capture it");
    check(capture.winner == StoneColor::Black, "capturing player should win Atari Go");
    check(board.capturedStones().count({3, 4}) != 0, "captured chain should include the surrounded stone");
}

void marksChainsInAtari() {
    Board board(9);

    check(board.placeStone({2, 4}, StoneColor::Black).legal, "reducing white to one liberty should be legal");
    check(board.atariStones().count({3, 4}) != 0, "a chain with one liberty should be marked as atari");
}

void concedingHandsTheWinToTheOpponent() {
    Game game;

    game.currentPlayerConcedes();

    check(game.winner() == StoneColor::White, "black conceding should hand white the win");
    checkFalse(game.isRunning(), "conceding should end the game");
}

// ------------------------------------------------------------------ bot moves

void botTakesAnAvailableCapture() {
    Board board(9);

    check(board.placeStone({2, 5}, StoneColor::White).legal, "white should be able to threaten the black stone");
    const std::optional<Position> suggestion = board.suggestMove(StoneColor::White);

    check(suggestion.has_value(), "the bot should find a move");
    checkEquals(Position({3, 6}), *suggestion, "the bot should capture the chain it can capture");
}

void botSuggestsLegalOpeningMoves() {
    Board board(9);

    const std::optional<Position> suggestion = board.suggestMove(StoneColor::Black);

    check(suggestion.has_value(), "an open board should always offer a move");
    check(board.isEmpty(*suggestion), "the suggested move must be an empty intersection");
}

// ---------------------------------------------------------------- move parsing

void parsesMoveInputInAnyOrder() {
    const MoveRequest letterFirst = parseMoveInput("A1");
    const MoveRequest numberFirst = parseMoveInput("1a");

    check(letterFirst.kind == MoveRequest::Kind::Place, "A1 should be a placement");
    checkEquals(Position({0, 0}), letterFirst.position, "A1 should map to row A column 1");
    checkEquals(letterFirst.position, numberFirst.position, "1a and A1 should mean the same intersection");

    check(parseMoveInput("q").kind == MoveRequest::Kind::Quit, "q should quit");
    check(parseMoveInput("Q").kind == MoveRequest::Kind::Quit, "Q should quit");
    check(parseMoveInput("A11").kind == MoveRequest::Kind::Invalid, "over-long input should be rejected");
    check(parseMoveInput("AB").kind == MoveRequest::Kind::Invalid, "input without a digit should be rejected");
}

void labelsPositionsForDisplay() {
    checkEquals(std::string("A1"), Position({0, 0}).label(), "row 0 column 0 should print as A1");
    checkEquals(std::string("D5"), Position({3, 4}).label(), "row 3 column 4 should print as D5");
}

// ------------------------------------------------------- match / view plumbing

// Scripted front end used to observe what Match asks the view to do.
class FakeView : public GameView {
public:
    explicit FakeView(std::deque<MoveRequest> script) : script_(std::move(script)) {}

    void showWelcome() override {}
    void showInstructions() override {}
    void waitForStart() override {}
    std::string askPlayerName(const std::string&, const std::string& fallback) override { return fallback; }

    void clearScreen() override { ++clears; }
    void showTurnBanner(const Player& current, const std::string& lastMove) override {
        banners.push_back(current.name() + "|" + lastMove);
    }
    void showBoard(const Game&) override { ++boards; }
    void showMessage(const std::string& message) override { messages.push_back(message); }

    MoveRequest askMove(const Player&) override {
        if (script_.empty()) return MoveRequest::quit();
        const MoveRequest next = script_.front();
        script_.pop_front();
        return next;
    }
    bool askPlayAgain() override { return false; }

    void showScore(const Player&, const Player&) override {}
    void showFarewell() override {}

    int clears = 0;
    int boards = 0;
    std::vector<std::string> banners;
    std::vector<std::string> messages;

private:
    std::deque<MoveRequest> script_;
};

void clearsScreenAndReportsTheLastMoveEachTurn() {
    FakeView view({MoveRequest::place({0, 0}), MoveRequest::place({1, 0})});
    Match match(view, Player(StoneColor::Black, "Slate"), Player(StoneColor::White, "Shell"));

    match.run();

    check(view.clears >= 3, "the screen should be cleared before every human turn");
    checkEquals(std::size_t(3), view.banners.size(), "each human turn should show a banner");
    checkEquals(std::string("Slate|"), view.banners[0], "the first turn has no previous move to report");
    checkEquals(std::string("Shell|Slate (B) placed a stone at A1"), view.banners[1],
                "the second player should see what the first player played");
    checkEquals(std::string("Slate|Shell (W) placed a stone at B1"), view.banners[2],
                "the third turn should report the second player's move");
}

void reportsIllegalMovesWithoutLosingTheTurn() {
    FakeView view({MoveRequest::place({3, 4}), MoveRequest::invalid("Invalid input"), MoveRequest::place({0, 0})});
    Match match(view, Player(StoneColor::Black, "Slate"), Player(StoneColor::White, "Shell"));

    match.run();

    bool sawOccupied = false;
    bool sawInvalid = false;
    for (const std::string& message : view.messages) {
        if (message == "That space is occupied") sawOccupied = true;
        if (message == "Invalid input") sawInvalid = true;
    }
    check(sawOccupied, "occupied intersections should be reported to the player");
    check(sawInvalid, "unparsable input should be reported to the player");
    checkEquals(std::string("Shell|Slate (B) placed a stone at A1"), view.banners[1],
                "black should still own the turn until a legal move is played");
}

void quittingEndsTheMatchWithoutAWinner() {
    FakeView view({MoveRequest::quit()});
    Match match(view, Player(StoneColor::Black, "Slate"), Player(StoneColor::White, "Shell"));

    match.run();

    bool sawNoWinner = false;
    for (const std::string& message : view.messages) {
        if (message == "No winner, no points awarded.") sawNoWinner = true;
    }
    check(sawNoWinner, "quitting should award no points");
}

void awardsTheWinToTheCapturingPlayer() {
    FakeView view({MoveRequest::place({2, 4}), MoveRequest::place({0, 0}), MoveRequest::place({3, 3})});
    Match match(view, Player(StoneColor::Black, "Slate"), Player(StoneColor::White, "Shell"));

    match.run();

    bool sawWin = false;
    for (const std::string& message : view.messages) {
        if (message == "Slate wins!") sawWin = true;
    }
    check(sawWin, "the capturing player should win the round");
}

// -------------------------------------------------------------- console output

void consoleViewDrawsTheGrid() {
    std::istringstream in;
    std::ostringstream out;
    ConsoleView view(in, out);
    Game game;

    view.showBoard(game);
    const std::string rendered = out.str();

    check(rendered.find("   1   2   3") != std::string::npos, "the header should number the columns");
    check(rendered.find("A ") != std::string::npos, "rows should be labelled with letters");
    check(rendered.find("(B)") != std::string::npos, "the opening black stones should be drawn");
    check(rendered.find("(W)") != std::string::npos, "the opening white stones should be drawn");
}

void consoleViewClearsTheScreen() {
    std::istringstream in;
    std::ostringstream out;
    ConsoleView view(in, out);

    view.clearScreen();

    check(out.str().find("\033[2J") != std::string::npos, "clearing should emit the ANSI clear sequence");
}

}  // namespace

int main() {
    rejectsInvalidAndOccupiedMoves();
    switchesTurnsAfterLegalMove();
    rejectsMovesAfterGameEnds();
    recordsCaptures();
    marksChainsInAtari();
    concedingHandsTheWinToTheOpponent();
    botTakesAnAvailableCapture();
    botSuggestsLegalOpeningMoves();
    parsesMoveInputInAnyOrder();
    labelsPositionsForDisplay();
    clearsScreenAndReportsTheLastMoveEachTurn();
    reportsIllegalMovesWithoutLosingTheTurn();
    quittingEndsTheMatchWithoutAWinner();
    awardsTheWinToTheCapturingPlayer();
    consoleViewDrawsTheGrid();
    consoleViewClearsTheScreen();

    if (failures != 0) {
        std::cout << "Failed " << failures << " of " << checks << " assertions.\n";
        return 1;
    }
    std::cout << "Passed " << checks << " assertions.\n";
    return 0;
}