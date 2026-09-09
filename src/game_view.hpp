#pragma once

#include <string>

#include "position.hpp"

namespace atarigo {

class Game;
class Player;

// What the player asked for on their turn.
struct MoveRequest {
    enum class Kind { Place, Quit, Invalid };

    Kind kind = Kind::Invalid;
    Position position{};
    std::string error;

    static MoveRequest place(Position position) { return {Kind::Place, position, {}}; }
    static MoveRequest quit() { return {Kind::Quit, {}, {}}; }
    static MoveRequest invalid(std::string error) { return {Kind::Invalid, {}, std::move(error)}; }
};

// Everything the Match needs from a front end. ConsoleView is the terminal
// implementation; GuiView is the stub kept for the future graphical build.
// Match never touches std::cout or std::cin directly, so a new front end only
// has to implement this interface.
class GameView {
public:
    virtual ~GameView() = default;

    // Setup.
    virtual void showWelcome() = 0;
    virtual void showInstructions() = 0;
    virtual void waitForStart() = 0;
    virtual std::string askPlayerName(const std::string& prompt, const std::string& fallback) = 0;

    // Per turn presentation.
    virtual void clearScreen() = 0;
    virtual void showTurnBanner(const Player& current, const std::string& lastMove) = 0;
    virtual void showBoard(const Game& game) = 0;
    virtual void showMessage(const std::string& message) = 0;

    // Interaction.
    virtual MoveRequest askMove(const Player& current) = 0;
    virtual bool askPlayAgain() = 0;

    // Teardown.
    virtual void showScore(const Player& first, const Player& second) = 0;
    virtual void showFarewell() = 0;
};

}  // namespace atarigo