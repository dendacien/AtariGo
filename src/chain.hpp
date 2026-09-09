#pragma once

#include <cstddef>
#include <set>

#include "position.hpp"

namespace atarigo {

// A connected group of same coloured stones together with its empty neighbours.
struct Chain {
    std::set<Position> stones;
    std::set<Position> liberties;

    std::size_t libertyCount() const { return liberties.size(); }
};

}  // namespace atarigo