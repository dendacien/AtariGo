#pragma once

#include <string>
#include <utility>

#include "stone_color.hpp"

namespace atarigo {

// Plain data about one participant. Turn logic lives in Match so the same
// Player works for a console or a graphical front end.
class Player {
public:
    Player(StoneColor color, std::string name)
        : color_(color), name_(std::move(name)), bot_(name_ == "Computer" && color == StoneColor::White) {}

    StoneColor color() const { return color_; }
    const std::string& name() const { return name_; }
    std::string_view symbol() const { return symbolOf(color_); }
    int wins() const { return wins_; }
    void recordWin() { ++wins_; }
    bool isBot() const { return bot_; }

private:
    StoneColor color_;
    std::string name_;
    int wins_ = 0;
    bool bot_ = false;
};

}  // namespace atarigo