#include <iostream>
#include <string>

#include "console_view.hpp"
#include "gui_view.hpp"
#include "match.hpp"
#include "player.hpp"
#include "stone_color.hpp"

namespace {

bool wantsGui(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--gui") return true;
    }
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    using namespace atarigo;

    // The GUI front end is still a stub, so fall back to the console for now.
    if (wantsGui(argc, argv) && !GuiView::isAvailable()) {
        std::cout << "The graphical front end is not implemented yet, starting the console version.\n";
    }

    ConsoleView view(std::cin, std::cout);
    view.showWelcome();
    view.showInstructions();
    view.waitForStart();

    Player black(StoneColor::Black, 
        view.askPlayerName("Name for Player 1 (default Slate): ", "Slate"));
    Player white(StoneColor::White,
        view.askPlayerName("Name for Player 2 (default Shell)(enter Computer for a bot): ", "Shell"));

    Match match(view, std::move(black), std::move(white));
    match.run();

    view.showFarewell();
    return 0;
}