#include "console_view.hpp"

#include <cctype>
#include <istream>
#include <ostream>
#include <string>

#include "game.hpp"
#include "player.hpp"

namespace atarigo {
namespace {

constexpr std::string_view kRed = "\033[31m";
constexpr std::string_view kYellow = "\033[33m";
constexpr std::string_view kCyan = "\033[36m";
constexpr std::string_view kPurple = "\033[35m";
constexpr std::string_view kReset = "\033[0m";

std::string paint(std::string_view color, std::string_view text) {
    return std::string(color) + std::string(text) + std::string(kReset);
}

}  // namespace

MoveRequest parseMoveInput(const std::string& text) {
    if (text.find('q') != std::string::npos || text.find('Q') != std::string::npos) return MoveRequest::quit();

    const std::string error = "Invalid input, must be one number and one letter";
    if (text.size() != 2) return MoveRequest::invalid(error);

    int row = 0;
    int col = 0;
    for (const char c : text) {
        const auto uc = static_cast<unsigned char>(c);
        if (std::isdigit(uc)) col = c - '0';
        else if (std::isalpha(uc)) row = std::tolower(uc) - 'a' + 1;
    }
    if (row == 0 || col == 0) return MoveRequest::invalid(error);
    return MoveRequest::place({row - 1, col - 1});
}

ConsoleView::ConsoleView(std::istream& in, std::ostream& out) : in_(in), out_(out) {}

bool ConsoleView::readLine(const std::string& prompt, std::string& line) {
    out_ << prompt << std::flush;
    if (!std::getline(in_, line)) {
        line.clear();
        return false;
    }
    return true;
}

std::string ConsoleView::colorFor(StoneColor color, std::string_view text) const {
    if (color == StoneColor::Black) return paint(kPurple, text);
    if (color == StoneColor::White) return paint(kCyan, text);
    return std::string(text);
}

void ConsoleView::showWelcome() { out_ << "Welcome to Atari Go!\n"; }

void ConsoleView::showInstructions() {
    out_ << "Stones are represented by " << paint(kCyan, "(W)") << " for " << paint(kCyan, "white") << " and "
         << paint(kPurple, "(B)") << " for " << paint(kPurple, "black") << ".\n"
         << "This game is played on a 9x9 grid. To place a stone give the column and row you wish to place your "
            "stone.\n"
         << "You can enter this in any order and any capitalization: 1A, A1, a1, etc\n"
         << "Player 1 will go first and will play " << paint(kPurple, "black (B)") << ".\n"
         << "Any stone can be placed in an empty space and if placing that stone will not result in immediate "
            "capture.\n"
         << "A stone is captured as soon as it has no more \"liberties\" which are empty spaces directly from the\n"
         << "stone. No diagonal directions are considered in Go. Stones that are touching vertically or hoizonally\n"
         << "share liberties. Any stone or chain of stones that has one liberty is said to be in \"atari\".\n"
         << "Stones in atari will be shown in " << paint(kYellow, "yellow") << ".\n"
         << "The player that captures the first stone wins the game. Captured stones will be shown in "
         << paint(kRed, "red") << ".\n"
         << "Enter q or Q in any move to quit.\n";
}

void ConsoleView::waitForStart() {
    std::string ignored;
    readLine("Press Enter to start", ignored);
}

std::string ConsoleView::askPlayerName(const std::string& prompt, const std::string& fallback) {
    std::string name;
    if (!readLine(prompt, name) || name.empty()) return fallback;
    return name;
}

void ConsoleView::clearScreen() {
    // Clear the screen and the scrollback, then home the cursor
    // For clean display of the board and game state
    out_ << "\033[2J\033[3J\033[H" << std::flush;
}

void ConsoleView::showTurnBanner(const Player& current, const std::string& lastMove) {
    out_ << "Atari Go\n";
    out_ << "Last move: " << (lastMove.empty() ? "none yet, black opens the game" : lastMove) << "\n";
    out_ << "Current player is " << current.name() << " " << colorFor(current.color(), current.symbol()) << "\n";
}

void ConsoleView::showBoard(const Game& game) {
    const int size = game.boardSize();
    printHeader(size);
    for (int row = 0; row < size; ++row) {
        printRow(game, row);
        if (row < size - 1) printSeparator(size);
    }
}

void ConsoleView::printHeader(int size) {
    for (int i = 1; i <= size; ++i) out_ << "   " << i;
    out_ << "\n";
}

void ConsoleView::printRow(const Game& game, int row) {
    out_ << static_cast<char>('A' + row) << " ";
    const std::set<Position>& captures = game.captures();
    const std::set<Position>& ataris = game.atariPositions();

    const int size = game.boardSize();
    for (int col = 0; col < size; ++col) {
        const Position stone{row, col};
        const std::string_view symbol = game.intersectionSymbol(stone);

        std::string intersection;
        if (captures.count(stone) != 0) intersection = paint(kRed, symbol);
        else if (ataris.count(stone) != 0) intersection = paint(kYellow, symbol);
        else if (symbol.find('B') != std::string_view::npos) intersection = paint(kPurple, symbol);
        else if (symbol.find('W') != std::string_view::npos) intersection = paint(kCyan, symbol);
        else intersection = std::string(symbol);

        if (col + 1 != size) intersection += "-";
        out_ << intersection;
    }
    out_ << "\n";
}

void ConsoleView::printSeparator(int size) {
    for (int col = 0; col < size; ++col) out_ << "   |";
    out_ << "\n";
}

void ConsoleView::showMessage(const std::string& message) {
    if (!message.empty()) out_ << message << "\n";
}

MoveRequest ConsoleView::askMove(const Player& current) {
    std::string line;
    if (!readLine(colorFor(current.color(), current.symbol()) + " move: ", line)) return MoveRequest::quit();
    return parseMoveInput(line);
}

bool ConsoleView::askPlayAgain() {
    std::string answer;
    if (!readLine("Play another round? ", answer)) return false;
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}

void ConsoleView::showScore(const Player& first, const Player& second) {
    out_ << first.name() << ": " << first.wins() << "\n";
    out_ << second.name() << ": " << second.wins() << "\n";
}

void ConsoleView::showFarewell() { out_ << "Thank you for playing!\n"; }

}  // namespace atarigo