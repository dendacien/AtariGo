#pragma once

#include <iosfwd>
#include <string>

#include "game_view.hpp"
#include "stone_color.hpp"

namespace atarigo {

// Terminal front end: ANSI colours, a text grid and line based input.
class ConsoleView : public GameView {
public:
    ConsoleView(std::istream& in, std::ostream& out);

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

private:
    // Returns false when input is exhausted, which is treated as quitting.
    bool readLine(const std::string& prompt, std::string& line);
    std::string colorFor(StoneColor color, std::string_view text) const;
    void printHeader(int size);
    void printRow(const Game& game, int row);
    void printSeparator(int size);

    std::istream& in_;
    std::ostream& out_;
};

// Parses input such as "1A", "a1" or "A1" into a board position.
// Exposed for testing; column is the digit, row is the letter.
MoveRequest parseMoveInput(const std::string& text);

}  // namespace atarigo