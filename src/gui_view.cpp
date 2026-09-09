#include "gui_view.hpp"

#include "game.hpp"
#include "player.hpp"

namespace atarigo {

// TODO(gui): create the main window and load the board assets.
void GuiView::showWelcome() {}

// TODO(gui): show the rules in a scrollable help panel.
void GuiView::showInstructions() {}

// TODO(gui): wait for the "Start" button.
void GuiView::waitForStart() {}

// TODO(gui): read the name from a text field, defaulting to `fallback`.
std::string GuiView::askPlayerName(const std::string& /*prompt*/, const std::string& fallback) { return fallback; }

// TODO(gui): hide the board while the seat is handed to the next player.
void GuiView::clearScreen() {}

// TODO(gui): render `lastMove` and the current player in the status bar.
void GuiView::showTurnBanner(const Player& /*current*/, const std::string& /*lastMove*/) {}

// TODO(gui): repaint the grid, colouring captured and atari stones.
void GuiView::showBoard(const Game& /*game*/) {}

// TODO(gui): surface the message in the status bar.
void GuiView::showMessage(const std::string& /*message*/) {}

// TODO(gui): map a click on an intersection to MoveRequest::place().
MoveRequest GuiView::askMove(const Player& /*current*/) { return MoveRequest::quit(); }

// TODO(gui): ask with a modal dialog.
bool GuiView::askPlayAgain() { return false; }

// TODO(gui): update the scoreboard widget.
void GuiView::showScore(const Player& /*first*/, const Player& /*second*/) {}

// TODO(gui): close the window.
void GuiView::showFarewell() {}

}  // namespace atarigo