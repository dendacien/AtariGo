#include "board.hpp"

#include <deque>
#include <random>

namespace atarigo {
namespace {

std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

Position pickRandom(const std::vector<Position>& moves) {
    std::uniform_int_distribution<std::size_t> pick(0, moves.size() - 1);
    return moves[pick(rng())];
}

void eraseAll(std::set<Position>& target, const std::set<Position>& toRemove) {
    for (const Position& stone : toRemove) target.erase(stone);
}

}  // namespace

Board::Board(int size)
    : size_(size), grid_(static_cast<std::size_t>(size) * static_cast<std::size_t>(size), StoneColor::Empty) {
    if (size != 9 && size != 13 && size != 19) throw std::invalid_argument("board must be 9x9, 13x13, or 19x19");

    // Opening stones for Atari Go.
    int center = (size / 2);
    setStone({center - 1, center}, StoneColor::White);
    setStone({center - 1, center + 1}, StoneColor::Black);
    setStone({center, center}, StoneColor::Black);
    setStone({center, center + 1}, StoneColor::White);
}

StoneColor Board::at(Position stone) const {
    if (!isOnBoard(stone)) throw std::out_of_range("position " + stone.label() + " is off the board");
    return stoneAt(stone);
}

bool Board::isOnBoard(Position stone) const {
    return stone.row >= 0 && stone.row < size_ && stone.col >= 0 && stone.col < size_;
}

bool Board::isEmpty(Position stone) const { return isOnBoard(stone) && stoneAt(stone) == StoneColor::Empty; }

MoveResult Board::placeStone(Position stone, StoneColor player) {
    if (!isOnBoard(stone)) return MoveResult::illegal("Out of bounds");
    if (!isEmpty(stone)) return MoveResult::illegal("That space is occupied");

    // The chain the stone would join once played.
    const Chain playerChain = findChain(stone, player);

    for (const Position& enemyStart : orthogonalEnemyNeighbors(stone, player)) {
        const Chain enemyChain = findChain(enemyStart, opponentOf(player));
        if (enemyChain.libertyCount() == 1) {
            eraseAll(atari_, playerChain.stones);
            setStone(stone, player);
            captured_.insert(enemyChain.stones.begin(), enemyChain.stones.end());
            return MoveResult::capturing(player);
        }
        if (enemyChain.libertyCount() == 2 && playerChain.libertyCount() > 1) {
            atari_.insert(enemyChain.stones.begin(), enemyChain.stones.end());
        }
    }

    if (playerChain.libertyCount() < 1) return MoveResult::illegal("Can not place into self capture");

    setStone(stone, player);
    if (playerChain.libertyCount() > 1) eraseAll(atari_, playerChain.stones);
    else atari_.insert(stone);
    return MoveResult::placed("placed a stone at " + stone.label());
}

bool Board::canPlaceStone(Position candidate, StoneColor player) {
    const std::set<Position> capturedSnapshot = captured_;
    const std::set<Position> atariSnapshot = atari_;

    const MoveResult result = placeStone(candidate, player);
    if (result.legal) setStone(candidate, StoneColor::Empty);

    captured_ = capturedSnapshot;
    atari_ = atariSnapshot;
    return result.legal;
}

Chain Board::findChain(Position start, StoneColor color) const {
    Chain chain;
    std::set<Position> visited{start};
    std::deque<Position> frontier{start};

    while (!frontier.empty()) {
        const Position current = frontier.front();
        frontier.pop_front();
        chain.stones.insert(current);

        for (const Position& neighbor : orthogonalNeighbors(current)) {
            if (visited.count(neighbor) != 0) continue;
            const StoneColor neighborColor = stoneAt(neighbor);
            if (neighborColor == StoneColor::Empty) {
                chain.liberties.insert(neighbor);
            } else if (neighborColor == color) {
                visited.insert(neighbor);
                frontier.push_back(neighbor);
            }
        }
    }
    return chain;
}

std::vector<Position> Board::orthogonalNeighbors(Position start) const {
    std::vector<Position> out;
    out.reserve(4);
    for (const Position& candidate : {Position{start.row - 1, start.col},
                                      Position{start.row, start.col + 1},
                                      Position{start.row + 1, start.col},
                                      Position{start.row, start.col - 1}}) {
        if (isOnBoard(candidate)) out.push_back(candidate);
    }
    return out;
}

std::vector<Position> Board::orthogonalEnemyNeighbors(Position origin, StoneColor player) const {
    std::vector<Position> starts;
    const StoneColor enemy = opponentOf(player);
    for (const Position& neighbor : orthogonalNeighbors(origin)) {
        if (stoneAt(neighbor) == enemy) starts.push_back(neighbor);
    }
    return starts;
}

std::optional<Position> Board::suggestMove(StoneColor player) {
    // Saving one of our own chains from atari always comes first.
    for (const Position& threatened : atari_) {
        if (stoneAt(threatened) != player) continue;
        const Chain chain = findChain(threatened, player);
        if (!chain.liberties.empty()) return *chain.liberties.begin();
    }

    std::vector<Position> threatenMoves;
    std::vector<Position> contactMoves;
    std::vector<Position> quietMoves;

    for (int row = 0; row < size_; ++row) {
        for (int col = 0; col < size_; ++col) {
            const Position candidate{row, col};
            if (!isEmpty(candidate)) continue;

            const std::vector<Position> enemyStarts = orthogonalEnemyNeighbors(candidate, player);
            if (!canPlaceStone(candidate, player)) continue;
            if (enemyStarts.empty()) {
                quietMoves.push_back(candidate);
                continue;
            }

            bool captures = false;
            bool threatens = false;
            for (const Position& enemyStart : enemyStarts) {
                const std::size_t liberties = findChain(enemyStart, opponentOf(player)).libertyCount();
                if (liberties == 1) captures = true;
                else if (liberties == 2) threatens = true;
            }

            if (captures) return candidate;
            if (threatens) threatenMoves.push_back(candidate);
            else contactMoves.push_back(candidate);
        }
    }

    if (!threatenMoves.empty()) return pickRandom(threatenMoves);
    if (!contactMoves.empty()) return pickRandom(contactMoves);
    if (!quietMoves.empty()) return pickRandom(quietMoves);
    return std::nullopt;
}

}  // namespace atarigo