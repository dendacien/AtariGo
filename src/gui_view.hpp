#pragma once

#include <string>

#include "game_view.hpp"

namespace atarigo {

// Stub front end reserved for the future graphical build.
//
// Match talks to GameView only, so turning Atari Go into a GUI application is
// a matter of filling in the methods below with a real toolkit (Qt, SDL,
// Dear ImGui, ...) and constructing a GuiView instead of a ConsoleView in
// main.cpp. Nothing in board.cpp / game.cpp / match.cpp needs to change.
//
// Suggested mapping once a toolkit is chosen:
//   showBoard        -> redraw the grid widget from Game state
//   askMove          -> translate a click on an intersection into a Position,
//                       or MoveRequest::quit() when the window is closed
//   clearScreen      -> hide the board while control passes to the next player
//   showTurnBanner   -> update the status bar with the last move details
//   showMessage      -> status bar text or a toast
//   askPlayAgain     -> modal dialog
//
// Every method is currently a no-op returning a safe default so the class
// compiles and can be linked into experiments without pulling in a toolkit.
class GuiView : public GameView {
public:
    // Returns false until a real graphical backend is implemented; main.cpp
    // uses this to fall back to the console front end.
    static bool isAvailable() { return false; }

    void showWelcome() override;
    void showInstructions() override;
    void waitForStart() override;
    std::string askPlayerName(const std::string& prompt, const std::string& fallback) override;

    void clearScreen() override;
    void showTurnBanner(const Player& current, const std::string& lastMove) override;
    void showBoard(const Game& game) override;
    void showMessage(const std::string& message) override;

    MoveRequest askMove(const Player& current) override;
    bool askPlayAgain() override;

    void showScore(const Player& first, const Player& second) override;
    void showFarewell() override;
};

}  // namespace atarigo