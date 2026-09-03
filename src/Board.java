package src;
import java.util.*;

public class Board {
    private final StoneColor[][] grid;
    private final int size;
    private final Set<Position> capturedChain;
    private final Set<Position> atariChains;

    public Board(int size) {
        this.grid = new StoneColor[size][size];
        this.size = size;
        this.capturedChain = new HashSet<>();
        this.atariChains = new HashSet<>();

        for (int row = 0; row < size; row++) {
            for(int col = 0; col <size; col++) {
                grid[row][col] = StoneColor.EMPTY;
            }
        }

        // Place first four stones for Atari Go
        grid[3][4] = StoneColor.WHITE;
        grid[3][5] = StoneColor.BLACK;
        grid[4][4] = StoneColor.BLACK;
        grid[4][5] = StoneColor.WHITE;
    }

    public MoveResult placeStone(Position stone, StoneColor player) {
        if(!isOnBoard(stone)) return MoveResult.illegal("Out of bounds");
        if(!isEmpty(stone)) return MoveResult.illegal("That space is occupied");

        ChainInfo playerChain = findChain(stone, player);
        for (Position enemyStart : cardinalEnemyNeighbors(stone, player)) {
            ChainInfo enemyChain = findChain(enemyStart, player.opponent());
            if (enemyChain.libertyCount() == 1) {
                atariChains.removeAll(playerChain.listStones());
                recordStoneOnBoard(stone, player);
                capturedChain.addAll(enemyChain.listStones());
                return MoveResult.legalCapture(player);
            } else if (enemyChain.libertyCount() == 2 && playerChain.libertyCount() > 1) {
                atariChains.addAll(enemyChain.listStones());
            }
        }

        if (playerChain.libertyCount() < 1) {
            return MoveResult.illegal("Can not place into self capture");
        } else {
            recordStoneOnBoard(stone, player);
            if (playerChain.libertyCount() > 1) atariChains.removeAll(playerChain.listStones());
            else atariChains.add(stone);
            return MoveResult.legalNoCapture("placed a stone at " + stone.toString());
        }
    }

    // A legal move can sometimes depend on if a capture happens and that makes it easier to simulate like this if only checking for the legalness of a move
    private boolean canPlaceStone(Position candidate, StoneColor player) {
        Set<Position> capturedSnapshot = new HashSet<>(capturedChain);
        Set<Position> atariSnapshot = new HashSet<>(atariChains);

        MoveResult result = placeStone(candidate, player);
        if (result.isLegal()) {
            removeStone(candidate);
        }

        capturedChain.clear();
        capturedChain.addAll(capturedSnapshot);
        atariChains.clear();
        atariChains.addAll(atariSnapshot);

        return result.isLegal();
    }

    private void recordStoneOnBoard(Position stone, StoneColor player) {
        grid[stone.row][stone.col] = player;
    }

    private void removeStone(Position stone) {
        grid[stone.row][stone.col] = StoneColor.EMPTY;
    }

    public int getSize() {
        return size;
    }

    public StoneColor getStoneAtPos(Position stone) {
        return grid[stone.row][stone.col];
    }

    public StoneColor get(Position stone) {
        if (!isOnBoard(stone)) throw new IndexOutOfBoundsException();
        return getStoneAtPos(stone);
    }

    public boolean isOnBoard(Position stone) {
        return stone.row >= 0 && stone.row < size && stone.col >= 0 && stone.col < size;
    }

    public boolean isEmpty(Position stone) {
        return isOnBoard(stone) && getStoneAtPos(stone) == StoneColor.EMPTY;
    }

    private ChainInfo findChain(Position start, StoneColor startColor) {
        ChainInfo info = new ChainInfo();

        Set<Position> visited = new HashSet<>();
        Deque<Position> frontier = new ArrayDeque<>();
        frontier.add(start);
        visited.add(start);

        while (!frontier.isEmpty()) {
            Position current = frontier.removeFirst();
            info.addStone(current);

            for (Position neighbor : cardinalNeighbors(current)) {
                if (visited.contains(neighbor)) continue;
                StoneColor neighborColor = getStoneAtPos(neighbor);
                if (neighborColor == StoneColor.EMPTY) {
                    info.addLiberty(neighbor);
                } else if (neighborColor == startColor) {
                    visited.add(neighbor);
                    frontier.addLast(neighbor);
                }
            }
        }
        return info;
    }

    private List<Position> cardinalNeighbors(Position start) {
        List<Position> out = new ArrayList<>();
        Position north = new Position(start.row -1, start.col);
        Position east = new Position(start.row, start.col +1);
        Position south = new Position(start.row + 1, start.col);
        Position west = new Position(start.row, start.col - 1);
        if (isOnBoard(north)) out.add(north);
        if (isOnBoard(east)) out.add(east);
        if (isOnBoard(south)) out.add(south);
        if (isOnBoard(west)) out.add(west);
        return out;
    }

    private List<Position> cardinalEnemyNeighbors(Position origin, StoneColor player) {
        List<Position> starts = new ArrayList<>();
        StoneColor enemy = player.opponent();
        for (Position neighbor : cardinalNeighbors(origin)) {
            if (getStoneAtPos(neighbor) == enemy) starts.add(neighbor);
        }
        return starts;
    }

    public Set<Position> getCatpured() {
        return capturedChain;
    }

    public Set<Position> getAtari() {
        return atariChains;
    }

    public Position suggestMove(StoneColor player) {
        List<Position> suggestAtari = new ArrayList<>();
        List<Position> suggestPressure = new ArrayList<>();
        List<Position> randomMoves = new ArrayList<>();

        if(!atariChains.isEmpty()) {
            for(Position atariCheck: atariChains) {
                if (getStoneAtPos(atariCheck) == player) {
                    ChainInfo atariChain = findChain(atariCheck, player);
                    return atariChain.listLiberties().iterator().next();
                }
            }
        }

        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                Position candidate = new Position(row, col);
                if (!isEmpty(candidate)) continue;

                List<Position> enemyStarts = cardinalEnemyNeighbors(candidate, player);
                if (!canPlaceStone(candidate, player)) continue;
                if (enemyStarts.isEmpty()) {
                    randomMoves.add(candidate);
                    continue;
                }

                boolean capturesEnemy = false;
                boolean putsInAtari = false;
                for (Position enemyStart : enemyStarts) {
                    int enemyLiberties = findChain(enemyStart, player.opponent()).libertyCount();
                    if (enemyLiberties == 1) capturesEnemy = true;
                    else if (enemyLiberties == 2) putsInAtari = true;
                }

                if (capturesEnemy) return candidate;
                if (putsInAtari) suggestAtari.add(candidate);
                else suggestPressure.add(candidate);
            }
        }

        if (!suggestAtari.isEmpty()) {
            Collections.shuffle(suggestAtari);
            return suggestAtari.get(0);
        }
        else if (!suggestPressure.isEmpty()) {
            Collections.shuffle(suggestPressure);
            return suggestPressure.get(0);
        }
        else {
            Collections.shuffle(randomMoves);
            return randomMoves.get(0);
        }
    }
}
